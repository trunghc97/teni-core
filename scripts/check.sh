#!/usr/bin/env sh
set -eu
pio test -e native
pio run -e teni_esp32s3

