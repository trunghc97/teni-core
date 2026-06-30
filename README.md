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

## Build firmware

1. Put story input (`.txt`, `.mp3`, or `.wav`) in `content/stories/`. Put other audio in
   `assets/music/`, `assets/sfx/`, or `assets/responses/`.
2. Add or edit JSON object files in `knowledge/` and response templates in
   `assets/responses/templates.json`.
3. Run `pio run -e teni_esp32s3`. PlatformIO automatically generates missing/stale story MP3s,
   then regenerates `assets.h`, `assets.cpp`, `build/asset_index.json`, and
   `build/flash_report.json`.

Run `pio test -e native` for host tests. The generated binary arrays are committed so IDE indexing
and clean checkouts work, but they must never be edited manually.

## Flash firmware to ESP32-S3

Requirements:

- Teni ESP32-S3 N16R8 board (16 MB flash and 8 MB PSRAM)
- a data-capable USB cable
- PlatformIO Core (`pio`) or the PlatformIO IDE extension

From the repository root, run the complete pre-flash check:

```sh
scripts/check.sh
```

This runs native tests and builds the `teni_esp32s3` firmware. Check that the final environment status
is `SUCCESS`, then inspect the available application space:

```sh
cat build/flash_report.json
```

Connect the board and find its serial port:

```sh
pio device list
```

Typical ports are `/dev/cu.usbmodem*` on macOS, `/dev/ttyACM*` on Linux, and `COM3` or similar on
Windows. PlatformIO normally detects the port automatically:

```sh
pio run -e teni_esp32s3 -t upload
```

If more than one serial device is connected, specify the port explicitly:

```sh
pio run -e teni_esp32s3 -t upload --upload-port /dev/cu.usbmodem1101
```

After upload, open the serial monitor at 115200 baud:

```sh
pio device monitor -p /dev/cu.usbmodem1101 -b 115200
```

Press `Ctrl+C` to close the monitor before uploading again. If automatic bootloader entry fails:

1. Hold the board's `BOOT` button.
2. Tap and release `RESET` while continuing to hold `BOOT`.
3. Start the upload command.
4. Release `BOOT` when PlatformIO prints `Connecting...`.

To completely erase flash, including NVS bookmarks and settings, then reinstall firmware:

```sh
pio run -e teni_esp32s3 -t erase --upload-port /dev/cu.usbmodem1101
pio run -e teni_esp32s3 -t upload --upload-port /dev/cu.usbmodem1101
```

Do not disconnect power while erasing or uploading. Prefer the PlatformIO `upload` target because it
writes the bootloader, partition table, and application at their correct offsets; the generated
`.pio/build/teni_esp32s3/firmware.bin` is not a complete factory flash bundle by itself.

See the [asset pipeline](docs/ASSET_PIPELINE.md), [architecture](docs/ARCHITECTURE.md),
[automatic TTS pipeline](docs/TTS_PIPELINE.md), [hardware wiring](docs/HARDWARE_WIRING.md), and
[detailed flash guide](docs/FLASH_GUIDE.md).
