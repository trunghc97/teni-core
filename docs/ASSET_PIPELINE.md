# Asset pipeline

`scripts/build_assets.py` scans `assets/` and `knowledge/` in sorted order, calculates SHA-256,
flattens knowledge and response JSON, and generates:

- `firmware/include/generated/assets.h`
- `firmware/src/generated/assets.cpp`
- `build/asset_index.json`

Every regular non-hidden file is embedded. MP3 and WAV are playable; JSON and text remain available
through `AssetManager`. WAV files larger than 2 MiB are converted to mono 22.05 kHz, 64 kbit/s MP3
when `ffmpeg` is installed; otherwise the build fails with an actionable message. This prevents a
large accidental PCM file from exhausting flash.

The combined asset budget is 12 MiB. The build fails before compilation if it is exceeded. Asset IDs
are extensionless relative paths such as `stories/story001`; paths retain their extension, for
example `/stories/story001.mp3`. Playback performs zero-copy reads from mapped flash through
`AudioFileSourcePROGMEM`.

Knowledge files must be JSON objects whose values are strings. They are converted into static lookup
records, so the device does not parse JSON or allocate a document at runtime.

## Flash layout

| Region | Offset | Size | Purpose |
|---|---:|---:|---|
| Firmware + embedded assets | `0x10000` | `0xEE0000` | Factory application image |
| NVS | `0xEF0000` | 64 KiB | Configuration and bookmark |
| Coredump | `0xF00000` | 64 KiB | Crash diagnostics |
| Future asset pack | `0xF10000` | 960 KiB | Reserved offline/USB asset update |

`scripts/flash_report.py` reports binary bytes, embedded asset bytes, partition capacity, and
remaining application space after every target build.

