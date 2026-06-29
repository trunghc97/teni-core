#!/usr/bin/env sh
set -eu
pio run -e teni_esp32s3 -t upload
pio device monitor -b 115200

