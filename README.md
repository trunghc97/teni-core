# teni-core

Offline embedded platform for the Teni smart-toy family. MVP1.1 runs entirely on an ESP32-S3 N16R8:
firmware, stories, music, response audio, and knowledge are compiled into its 16 MB flash. There is
no MicroSD, network, cloud, Linux, or Raspberry Pi dependency.

## MVP1.1 capabilities

- Embedded MP3/WAV playback through native ESP32 I2S and MAX98357A
- Story pause/resume/stop/next/previous/random/repeat with NVS bookmarks
- Music playlists, shuffle, and volume
- Deterministic asset pipeline with SHA-256 index and 12 MiB asset budget enforcement
- Fully offline `SimpleAIEngine`: priority, pattern, keyword, and fuzzy intent matching
- Flash-compiled knowledge for animals, colors, numbers, alphabet, math, greetings, and bedtime
- Replaceable `IAIEngine`; a future local LLM does not affect skills, assets, or the application
- `IPlugin`, `ISkill`, `BaseSkill`, Story/Music/Knowledge skills, and future skill placeholders
- Buttons, LED state, battery monitoring, NVS settings, deep sleep, and FreeRTOS event runtime

## Build and flash

1. Put story input (`.txt`, `.mp3`, or `.wav`) in `content/stories/`. Put other audio in
   `assets/music/`, `assets/sfx/`, or `assets/responses/`.
2. Add or edit JSON object files in `knowledge/` and response templates in
   `assets/responses/templates.json`.
3. Run `pio run -e teni_esp32s3`. PlatformIO automatically generates missing/stale story MP3s,
   then regenerates `assets.h`, `assets.cpp`, `build/asset_index.json`, and
   `build/flash_report.json`.
4. Upload with `pio run -e teni_esp32s3 -t upload` and monitor at 115200 baud.

Run `pio test -e native` for host tests. The generated binary arrays are committed so IDE indexing
and clean checkouts work, but they must never be edited manually.

See the [asset pipeline](docs/ASSET_PIPELINE.md), [architecture](docs/ARCHITECTURE.md),
[automatic TTS pipeline](docs/TTS_PIPELINE.md), [hardware wiring](docs/HARDWARE_WIRING.md), and
[MVP1 migration notes](docs/MIGRATION_MVP1_TO_MVP1_1.md).
