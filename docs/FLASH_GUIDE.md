# Flash guide

1. Connect the board over a known data-capable USB cable.
2. Run `scripts/check.sh` and require all tests plus the `teni_esp32s3` build to pass.
3. Check `build/flash_report.json` for application headroom.
4. Find the port with `pio device list`.
5. Upload with `pio run -e teni_esp32s3 -t upload`. Add `--upload-port <port>` when automatic port
   detection is ambiguous.
6. If auto-reset fails, hold BOOT, tap RESET, start upload, and release BOOT when `Connecting...`
   appears.
7. Inspect startup using `pio device monitor -p <port> -b 115200`.

To erase development data, run `pio run -e teni_esp32s3 -t erase`, then flash again. Erase removes
NVS bookmarks and configuration. Never disconnect power while flashing. Production units should
use secure boot, flash encryption, signed images, manufacturing keys, and a controlled provisioning
process; those are outside MVP1.1.

Use PlatformIO's upload target for a normal installation. It writes the bootloader, partition table,
and application at the correct offsets; `firmware.bin` alone is only the application image, not a
complete factory bundle.
