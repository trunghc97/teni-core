# Test suites

- `test_core`: event bus and device state machine.
- `test_button`: debouncing and gesture recognition with a mock GPIO reader.
- `test_integration`: story library, bookmark resume, and audio/config mocks.
- `test_hal`: storage and audio abstraction contract tests.
- `test_ai`: embedded asset registration, knowledge lookup, intent priority, fuzzy matching, and math.

Run all host tests with `pio test -e native`. Embedded hardware abstraction tests are performed
by flashing the `hw_test` build profile described in `docs/DEVELOPMENT.md`; the mock interfaces
keep the business rules testable without an ESP32.
