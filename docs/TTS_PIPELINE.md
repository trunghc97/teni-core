# Automatic story voice pipeline

Story input lives in `content/stories/`. Each story may be supplied as `.mp3`, `.wav`, or UTF-8
`.txt`. Files with the same relative stem represent the same story; an audio source takes precedence
over text (`.mp3`, then `.wav`, then `.txt`). The prepared output is always a mono 22.05 kHz,
48 kbit/s MP3 under `assets/stories/`.

`scripts/build_assets.py` runs this preparation automatically before embedding assets. Unchanged
outputs are reused, so normal firmware builds do not reload the TTS model. Editing a source or adding
a new source rebuilds only the affected story. To intentionally regenerate every story, run:

```sh
python3 scripts/prepare_story_audio.py --force
```

## Vietnamese voice

Text input uses the same default stack as `ai_video_subtitle_tool`:

- F5-TTS Base
- `hynt/F5-TTS-Vietnamese-ViVoice` checkpoint
- Vocos vocoder, CPU inference, NFE step 8
- a Northern Vietnamese reference voice, with Piper `vi_VN-vais1000-medium` as the automatic
  reference fallback

The build looks for reference audio in this order:

1. `TENI_F5_REF_AUDIO` (or `F5_REF_AUDIO_PATH`)
2. `content/voice/ref_north.wav`
3. the sibling `ai_video_subtitle_tool/storage/models/f5/ref_north.wav`
4. an automatically generated Piper reference cached under `.cache/teni-tts/`

For a consistent expressive voice, put a clean 8–15 second recording at
`content/voice/ref_north.wav` and set `TENI_F5_REF_TEXT` to its exact transcript. F5 clones the voice
and speaking style present in this reference; punctuation and paragraphing in story text also affect
delivery.

The first text synthesis builds a local Docker runtime and downloads the ViVoice checkpoint (about
5.4 GB). Runtime files and downloaded models are cached in `.cache/teni-tts/` and are not flashed to
the ESP32. The public ViVoice model card marks the checkpoint for non-commercial research use; replace
it with a suitably licensed checkpoint before commercial distribution.

Useful overrides:

```sh
TENI_F5_REF_AUDIO=/path/to/reference.wav
TENI_F5_REF_TEXT='Exact transcript of the reference recording.'
TENI_F5_SPEED=1.0
TENI_F5_NFE_STEP=8
TENI_TTS_FORCE=1
```

The ESP32 does not run F5-TTS. It receives only the generated MP3 and continues to decode it through
the existing `AudioEngine` and I2S output.
