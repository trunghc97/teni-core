# Flash guide

1. Connect the board over a known data-capable USB cable.
2. Build assets and firmware with `pio run -e teni_esp32s3`.
3. Check `build/flash_report.json` for application headroom.
4. Upload with `pio run -e teni_esp32s3 -t upload`.
5. If auto-reset fails, hold BOOT, tap RESET, start upload, then release BOOT.
6. Inspect startup at 115200 baud using `pio device monitor`.

To erase development data, run `pio run -e teni_esp32s3 -t erase`, then flash again. Erase removes
NVS bookmarks and configuration. Never disconnect power while flashing. Production units should
use secure boot, flash encryption, signed images, manufacturing keys, and a controlled provisioning
process; those are outside MVP1.1.
