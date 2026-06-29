# Development guide

## Workflow

1. Create a short-lived branch.
2. Run `scripts/format.sh`.
3. Run `scripts/check.sh` (host tests and ESP32 build).
4. Test on hardware with representative MP3 and WAV files.
5. Submit focused changes with documentation and tests.

## Adding a skill

Implement `ISkill`, depend only on interfaces/services, subscribe to typed events, register it in the
composition root, and add a host test with mocks. Do not make one skill call another directly. Add a
new event or coordinator policy when behavior crosses skills.

## Adding a codec

Add a decoder adapter/selection branch behind `IAudioEngine`; do not expose decoder library types to
modules. Add files to `MediaLibrary` extension discovery and test unsupported/corrupt input. AAC and
FLAC are planned but intentionally absent.

## Hardware abstraction tests

Driver behavior should be tested through interface contract fakes on the host. On target, run a
bench test build that checks embedded asset lookup/audio, LED colors, button edges, calibrated battery ADC,
I2S tone/output, charger detect, wake, and power-latch behavior. Destructive power tests must be last
and require a serial test fixture. Record board revision and ADC calibration in manufacturing data.

## Coding rules

C++17, RAII ownership, no mutable globals, no business rules in `main.cpp`, no `delay()`, no waiting
inside an event handler, and no allocation in high-frequency paths. Prefer fixed-size event payloads.
Doxygen public contracts and keep dependencies pointing inward.
