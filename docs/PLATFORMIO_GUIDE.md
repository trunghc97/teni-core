# PlatformIO guide

The default `teni_esp32s3` environment pins the Espressif platform, selects the DevKitC-1, enables
16 MB flash and 8 MB octal PSRAM, and builds with C++17. Libraries are version constrained in
`platformio.ini`.

```sh
pio run -e teni_esp32s3
pio run -e teni_esp32s3 -t upload
pio device monitor -b 115200
pio test -e native
```

In VS Code, open the repository root (not `firmware/`) and use the PlatformIO toolbar. If a board
variant uses a different PSRAM wiring mode, adjust `board_build.arduino.memory_type` only after
checking its schematic. Generate IDE metadata with `pio project init --ide vscode` if required.

The partition table dedicates 14.875 MiB to the factory application and its linked assets, plus NVS,
coredump, and a reserved offline asset-pack region. Internet OTA is intentionally unavailable.
