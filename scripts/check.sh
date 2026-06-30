#!/usr/bin/env sh
set -eu
python3 -m unittest discover -s scripts/tests -p 'test_*.py'
pio test -e native
pio run -e teni_esp32s3
