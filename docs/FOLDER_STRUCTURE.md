# Folder structure

```text
firmware/include/       Public C++ contracts and declarations
  app/                  Composition root
  config/               Board and persistent configuration models
  core/                 Event bus, state machine, logging, skill registry
  drivers/              ESP32/Arduino concrete adapters
  events/               Stable event value definitions
  interfaces/           Core and future AI boundaries
  modules/story|music/  Product skills
  services/             Reusable orchestration and policies
firmware/src/           Implementations mirroring include/
assets/                 Build-time audio, SFX, and response resources
knowledge/              Build-time offline knowledge JSON
boards/                 N16R8 PlatformIO board definition
hardware/               Wiring source files
scripts/                Developer commands
docs/                   Design and operations guides
test/                   Host tests and mock drivers
.github/workflows/      CI
```

The generated files under `firmware/*/generated` are refreshed automatically from `assets/` and
`knowledge/`. NVS is the only writable persistent store.
