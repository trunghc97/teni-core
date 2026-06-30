#!/usr/bin/env python3
"""Prepare deterministic story MP3 assets from audio files or Vietnamese text."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import time
import urllib.request
from typing import Callable


PROJECT = Path(__file__).resolve().parents[1]
CONTENT_ROOT = PROJECT / "content" / "stories"
ASSET_ROOT = PROJECT / "assets" / "stories"
CACHE_ROOT = PROJECT / ".cache" / "teni-tts"
RAW_ROOT = PROJECT / "build" / "story_audio"

F5_MODEL_URL = (
    "https://huggingface.co/hynt/F5-TTS-Vietnamese-ViVoice/resolve/main/model_last.pt"
)
F5_VOCAB_URL = (
    "https://huggingface.co/hynt/F5-TTS-Vietnamese-ViVoice/resolve/main/config.json"
)
PIPER_VOICE_NAME = "vi_VN-vais1000-medium"
PIPER_MODEL_URL = (
    "https://huggingface.co/rhasspy/piper-voices/resolve/main/vi/vi_VN/vais1000/"
    "medium/vi_VN-vais1000-medium.onnx"
)
PIPER_CONFIG_URL = PIPER_MODEL_URL + ".json"
DEFAULT_REF_TEXT = (
    "xin chào, đây là giọng đọc tiếng việt miền bắc dùng để lồng tiếng video."
)
DEFAULT_DOCKER_IMAGE = "teni-f5-tts:1.0"
MTIME_TOLERANCE_NS = 1_000_000_000


@dataclass(frozen=True)
class StorySource:
    relative_stem: Path
    source: Path
    kind: str


@dataclass(frozen=True)
class TextJob:
    source: Path
    raw_output: Path


Synthesizer = Callable[[list[TextJob], Path], None]
Normalizer = Callable[[Path, Path], None]


def _truthy(value: str | None) -> bool:
    return bool(value and value.strip().lower() in {"1", "true", "yes", "on"})


def _setting(name: str, default: str) -> str:
    return os.getenv(f"TENI_{name}", os.getenv(name, default))


def _run(command: list[str], *, input_text: str | None = None) -> None:
    completed = subprocess.run(command, input=input_text, text=True, check=False)
    if completed.returncode != 0:
        raise RuntimeError(f"command failed ({completed.returncode}): {' '.join(command)}")


def _download(url: str, destination: Path) -> None:
    if destination.exists() and destination.stat().st_size > 0:
        return
    destination.parent.mkdir(parents=True, exist_ok=True)
    temporary = destination.with_suffix(destination.suffix + ".tmp")
    temporary.unlink(missing_ok=True)
    print(f"teni tts: downloading {destination.name} from {url}")
    try:
        with urllib.request.urlopen(url, timeout=300) as response, temporary.open("wb") as output:
            expected = int(response.headers.get("Content-Length", "0"))
            downloaded = 0
            last_report = time.monotonic()
            while True:
                chunk = response.read(4 * 1024 * 1024)
                if not chunk:
                    break
                output.write(chunk)
                downloaded += len(chunk)
                if time.monotonic() - last_report >= 10:
                    if expected:
                        print(f"teni tts: {destination.name} {downloaded * 100 // expected}%")
                    else:
                        print(f"teni tts: {destination.name} {downloaded // (1024 * 1024)} MiB")
                    last_report = time.monotonic()
        temporary.replace(destination)
    except Exception:
        temporary.unlink(missing_ok=True)
        raise


def discover_sources(content_root: Path) -> list[StorySource]:
    """Find one effective input per story; MP3 wins over WAV, which wins over TXT."""
    candidates: dict[Path, dict[str, Path]] = {}
    if not content_root.exists():
        return []
    for path in sorted(item for item in content_root.rglob("*") if item.is_file()):
        if path.name.startswith(".") or path.suffix.lower() not in {".txt", ".mp3", ".wav"}:
            continue
        relative = path.relative_to(content_root)
        stem = relative.with_suffix("")
        candidates.setdefault(stem, {})[path.suffix.lower()] = path

    sources: list[StorySource] = []
    for stem, choices in sorted(candidates.items(), key=lambda item: item[0].as_posix()):
        for extension, kind in ((".mp3", "audio"), (".wav", "audio"), (".txt", "text")):
            if extension in choices:
                sources.append(StorySource(stem, choices[extension], kind))
                break
    return sources


def _needs_rebuild(source: Path, output: Path, force: bool) -> bool:
    return (
        force
        or not output.exists()
        or output.stat().st_size == 0
        or source.stat().st_mtime_ns > output.stat().st_mtime_ns + MTIME_TOLERANCE_NS
    )


def normalize_audio(source: Path, output: Path) -> None:
    ffmpeg = shutil.which("ffmpeg")
    if ffmpeg is None:
        raise RuntimeError("ffmpeg is required to prepare story audio")
    output.parent.mkdir(parents=True, exist_ok=True)
    temporary = output.with_suffix(".tmp.mp3")
    temporary.unlink(missing_ok=True)
    _run(
        [
            ffmpeg,
            "-v",
            "error",
            "-y",
            "-i",
            str(source),
            "-vn",
            "-ac",
            "1",
            "-ar",
            "22050",
            "-codec:a",
            "libmp3lame",
            "-b:a",
            "48k",
            "-map_metadata",
            "-1",
            str(temporary),
        ]
    )
    temporary.replace(output)


def _ensure_docker_image(project: Path) -> str:
    docker = shutil.which("docker")
    if docker is None:
        raise RuntimeError("Docker is required for automatic F5-TTS setup")
    daemon = subprocess.run(
        [docker, "info"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False
    )
    if daemon.returncode != 0:
        raise RuntimeError("Docker Desktop must be running for automatic F5-TTS setup")
    image = _setting("F5_DOCKER_IMAGE", DEFAULT_DOCKER_IMAGE)
    inspected = subprocess.run(
        [docker, "image", "inspect", image],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        check=False,
    )
    if inspected.returncode != 0:
        print(f"teni tts: building local F5-TTS runtime image {image}")
        _run(
            [
                docker,
                "build",
                "-f",
                str(project / "tools/tts/Dockerfile"),
                "-t",
                image,
                str(project),
            ]
        )
    return image


def _docker_path(path: Path, project: Path, cache_root: Path) -> str:
    resolved = path.resolve()
    try:
        return "/cache/" + resolved.relative_to(cache_root.resolve()).as_posix()
    except ValueError:
        return "/workspace/" + resolved.relative_to(project.resolve()).as_posix()


def _reference_audio(project: Path, cache_root: Path, image: str) -> Path:
    configured = os.getenv("TENI_F5_REF_AUDIO") or os.getenv("F5_REF_AUDIO_PATH")
    candidates = []
    if configured:
        candidates.append(Path(configured).expanduser())
    candidates.extend(
        [
            project / "content/voice/ref_north.wav",
            project.parent / "ai_video_subtitle_tool/storage/models/f5/ref_north.wav",
        ]
    )
    for candidate in candidates:
        if candidate.exists() and candidate.stat().st_size > 0:
            cached = cache_root / "f5" / "reference.wav"
            cached.parent.mkdir(parents=True, exist_ok=True)
            changed = not cached.exists()
            if not changed:
                changed = hashlib.sha256(candidate.read_bytes()).digest() != hashlib.sha256(
                    cached.read_bytes()
                ).digest()
            if changed:
                shutil.copy2(candidate, cached)
            return cached

    reference = cache_root / "f5" / "ref_north.wav"
    if reference.exists() and reference.stat().st_size > 0:
        return reference

    piper_dir = cache_root / "piper"
    model = piper_dir / f"{PIPER_VOICE_NAME}.onnx"
    config = piper_dir / f"{PIPER_VOICE_NAME}.onnx.json"
    _download(_setting("PIPER_MODEL_URL", PIPER_MODEL_URL), model)
    _download(_setting("PIPER_CONFIG_URL", PIPER_CONFIG_URL), config)
    reference.parent.mkdir(parents=True, exist_ok=True)
    print("teni tts: creating the same automatic Piper reference fallback as ai_video_subtitle_tool")
    docker = shutil.which("docker")
    assert docker is not None
    _run(
        [
            docker,
            "run",
            "--rm",
            "-i",
            "-v",
            f"{cache_root.resolve()}:/cache",
            image,
            "piper",
            "--model",
            _docker_path(model, project, cache_root),
            "--config",
            _docker_path(config, project, cache_root),
            "--output-file",
            _docker_path(reference, project, cache_root),
        ],
        input_text=_setting("F5_REF_TEXT", DEFAULT_REF_TEXT),
    )
    return reference


def synthesize_with_f5(jobs: list[TextJob], project: Path) -> None:
    if not jobs:
        return
    cache_root = project / ".cache" / "teni-tts"
    image = _ensure_docker_image(project)
    model = cache_root / "f5" / "model_last.pt"
    vocab = cache_root / "f5" / "vocab.txt"
    _download(_setting("F5_MODEL_URL", F5_MODEL_URL), model)
    _download(_setting("F5_VOCAB_URL", F5_VOCAB_URL), vocab)
    reference = _reference_audio(project, cache_root, image)

    payload = []
    for job in jobs:
        text = job.source.read_text(encoding="utf-8").strip()
        if not text:
            raise RuntimeError(f"story text is empty: {job.source}")
        job.raw_output.parent.mkdir(parents=True, exist_ok=True)
        payload.append({"text": text, "output": _docker_path(job.raw_output, project, cache_root)})
    job_file = cache_root / "f5" / "jobs.json"
    job_file.write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")

    docker = shutil.which("docker")
    assert docker is not None
    _run(
        [
            docker,
            "run",
            "--rm",
            "-v",
            f"{project.resolve()}:/workspace",
            "-v",
            f"{cache_root.resolve()}:/cache",
            image,
            "python",
            "/app/f5_batch.py",
            "--jobs",
            _docker_path(job_file, project, cache_root),
            "--model",
            _docker_path(model, project, cache_root),
            "--vocab",
            _docker_path(vocab, project, cache_root),
            "--reference",
            _docker_path(reference, project, cache_root),
            "--reference-text",
            _setting("F5_REF_TEXT", DEFAULT_REF_TEXT),
            "--speed",
            _setting("F5_SPEED", "1.0"),
            "--nfe-step",
            _setting("F5_NFE_STEP", "8"),
        ]
    )


def prepare_story_audio(
    project: Path = PROJECT,
    *,
    force: bool | None = None,
    synthesizer: Synthesizer | None = None,
    normalizer: Normalizer | None = None,
) -> tuple[int, int]:
    """Prepare changed story sources and return (audio_count, text_count)."""
    content_root = project / "content/stories"
    asset_root = project / "assets/stories"
    raw_root = project / "build/story_audio"
    force = _truthy(os.getenv("TENI_TTS_FORCE")) if force is None else force
    normalizer = normalizer or normalize_audio
    sources = discover_sources(content_root)
    audio_jobs: list[tuple[Path, Path]] = []
    text_jobs: list[tuple[TextJob, Path]] = []

    for story in sources:
        output = asset_root / story.relative_stem.with_suffix(".mp3")
        if not _needs_rebuild(story.source, output, force):
            continue
        if story.kind == "audio":
            audio_jobs.append((story.source, output))
        else:
            raw = raw_root / story.relative_stem.with_suffix(".wav")
            text_jobs.append((TextJob(story.source, raw), output))

    for source, output in audio_jobs:
        print(f"teni stories: normalizing {source.relative_to(project)}")
        normalizer(source, output)

    if text_jobs:
        print(
            f"teni stories: synthesizing {len(text_jobs)} text file(s) "
            "with F5-TTS Vietnamese ViVoice"
        )
        (synthesizer or synthesize_with_f5)([job for job, _ in text_jobs], project)
        for job, output in text_jobs:
            if not job.raw_output.exists() or job.raw_output.stat().st_size == 0:
                raise RuntimeError(f"F5-TTS did not create {job.raw_output}")
            normalizer(job.raw_output, output)

    if audio_jobs or text_jobs:
        print(
            f"teni stories: prepared {len(audio_jobs)} audio and "
            f"{len(text_jobs)} text input(s)"
        )
    return len(audio_jobs), len(text_jobs)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--force", action="store_true", help="regenerate every discovered story")
    args = parser.parse_args()
    prepare_story_audio(force=args.force)


if __name__ == "__main__":
    try:
        main()
    except Exception as exc:
        print(f"teni story audio failed: {exc}", file=sys.stderr)
        raise SystemExit(1) from exc
