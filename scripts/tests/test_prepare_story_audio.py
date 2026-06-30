from __future__ import annotations

import os
from pathlib import Path
import sys
import tempfile
import unittest


sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from prepare_story_audio import _docker_path, discover_sources, prepare_story_audio  # noqa: E402


class PrepareStoryAudioTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory()
        self.project = Path(self.temporary.name)
        (self.project / "content/stories").mkdir(parents=True)
        (self.project / "assets/stories").mkdir(parents=True)

    def tearDown(self) -> None:
        self.temporary.cleanup()

    def test_audio_input_wins_over_text_with_the_same_name(self) -> None:
        text = self.project / "content/stories/story.txt"
        audio = self.project / "content/stories/story.mp3"
        text.write_text("nội dung", encoding="utf-8")
        audio.write_bytes(b"mp3")

        sources = discover_sources(self.project / "content/stories")

        self.assertEqual(1, len(sources))
        self.assertEqual(audio, sources[0].source)
        self.assertEqual("audio", sources[0].kind)

    def test_mp3_input_is_automatically_normalized(self) -> None:
        source = self.project / "content/stories/story.mp3"
        source.write_bytes(b"source")
        calls: list[tuple[Path, Path]] = []

        def fake_normalize(input_path: Path, output_path: Path) -> None:
            calls.append((input_path, output_path))
            output_path.parent.mkdir(parents=True, exist_ok=True)
            output_path.write_bytes(b"normalized")

        counts = prepare_story_audio(self.project, normalizer=fake_normalize)

        self.assertEqual((1, 0), counts)
        self.assertEqual(source, calls[0][0])
        self.assertEqual(b"normalized", (self.project / "assets/stories/story.mp3").read_bytes())

    def test_text_input_runs_f5_then_normalizes(self) -> None:
        source = self.project / "content/stories/story.txt"
        source.write_text("Ngày xửa ngày xưa.", encoding="utf-8")
        synthesized: list[Path] = []

        def fake_synthesize(jobs, _project: Path) -> None:
            for job in jobs:
                synthesized.append(job.source)
                job.raw_output.parent.mkdir(parents=True, exist_ok=True)
                job.raw_output.write_bytes(b"wav")

        def fake_normalize(_input: Path, output: Path) -> None:
            output.parent.mkdir(parents=True, exist_ok=True)
            output.write_bytes(b"mp3")

        counts = prepare_story_audio(
            self.project, synthesizer=fake_synthesize, normalizer=fake_normalize
        )

        self.assertEqual((0, 1), counts)
        self.assertEqual([source], synthesized)
        self.assertEqual(b"mp3", (self.project / "assets/stories/story.mp3").read_bytes())

    def test_unchanged_output_is_reused(self) -> None:
        source = self.project / "content/stories/story.txt"
        output = self.project / "assets/stories/story.mp3"
        source.write_text("nội dung", encoding="utf-8")
        output.write_bytes(b"cached")
        newer = source.stat().st_mtime_ns + 1_000_000_000
        os.utime(output, ns=(newer, newer))

        def fail_synthesize(_jobs, _project: Path) -> None:
            self.fail("fresh output must not be synthesized")

        counts = prepare_story_audio(self.project, synthesizer=fail_synthesize)

        self.assertEqual((0, 0), counts)
        self.assertEqual(b"cached", output.read_bytes())

    def test_cache_paths_use_the_dedicated_container_mount(self) -> None:
        cache = self.project / ".cache/teni-tts"
        model = cache / "f5/model_last.pt"

        self.assertEqual("/cache/f5/model_last.pt", _docker_path(model, self.project, cache))


if __name__ == "__main__":
    unittest.main()
