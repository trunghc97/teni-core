#!/usr/bin/env sh
set -eu
find firmware test -type d -name generated -prune -o \
  -type f \( -name '*.cpp' -o -name '*.hpp' \) -print0 | xargs -0 clang-format -i
