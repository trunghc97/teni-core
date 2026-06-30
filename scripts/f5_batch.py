#!/usr/bin/env python3
"""Load F5-TTS once and synthesize a batch of story files."""

from __future__ import annotations

import argparse
import contextlib
from importlib.resources import files
import io
import json
from pathlib import Path
import warnings

import soundfile as sf


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--jobs", type=Path, required=True)
    parser.add_argument("--model", type=Path, required=True)
    parser.add_argument("--vocab", type=Path, required=True)
    parser.add_argument("--reference", type=Path, required=True)
    parser.add_argument("--reference-text", required=True)
    parser.add_argument("--speed", type=float, default=1.0)
    parser.add_argument("--nfe-step", type=int, default=8)
    args = parser.parse_args()

    from f5_tts.infer.utils_infer import (
        infer_process,
        load_model,
        load_vocoder,
        preprocess_ref_audio_text,
    )
    from hydra.utils import get_class
    from omegaconf import OmegaConf

    config = OmegaConf.load(str(files("f5_tts").joinpath("configs/F5TTS_Base.yaml")))
    model_class = get_class(f"f5_tts.model.{config.model.backbone}")
    with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
        vocoder = load_vocoder(vocoder_name="vocos", is_local=False, device="cpu")
        model = load_model(
            model_class,
            config.model.arch,
            str(args.model),
            mel_spec_type="vocos",
            vocab_file=str(args.vocab),
            device="cpu",
        )
        reference_audio, reference_text = preprocess_ref_audio_text(
            str(args.reference), args.reference_text, show_info=lambda _: None
        )

    jobs = json.loads(args.jobs.read_text(encoding="utf-8"))
    for index, job in enumerate(jobs, start=1):
        output = Path(job["output"])
        output.parent.mkdir(parents=True, exist_ok=True)
        print(f"teni tts: synthesizing {index}/{len(jobs)} -> {output.name}")
        with (
            warnings.catch_warnings(),
            contextlib.redirect_stdout(io.StringIO()),
            contextlib.redirect_stderr(io.StringIO()),
        ):
            warnings.simplefilter("ignore")
            audio, sample_rate, _ = infer_process(
                reference_audio,
                reference_text,
                job["text"],
                model,
                vocoder,
                mel_spec_type="vocos",
                show_info=lambda _: None,
                progress=None,
                nfe_step=args.nfe_step,
                speed=args.speed,
                device="cpu",
            )
        if audio is None:
            raise RuntimeError(f"F5-TTS returned no audio for {output.name}")
        sf.write(str(output), audio, sample_rate)


if __name__ == "__main__":
    main()
