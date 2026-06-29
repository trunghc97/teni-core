# Contributing

Keep pull requests small and explain observable behavior, architecture impact, hardware assumptions,
and verification. New functionality requires tests; pin or electrical changes require documentation.
Run formatting, native tests, and the embedded build before review.

Commit messages should be imperative and scoped where useful (`audio: handle corrupt WAV`). Avoid
drive-by refactors. Never commit credentials, signing keys, proprietary audio, build output, or device
serial data. Report battery, thermal, charging, or child-safety concerns privately to maintainers
before public discussion.

Reviewers check dependency direction, non-blocking behavior, bounded memory, error recovery,
persistence wear, event semantics, and compatibility with the ESP32-S3 resource budget.

