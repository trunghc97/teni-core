# Roadmap

## MVP1 — offline media foundation (implemented)

Story/music playback, gestures, event bus, state machine, NVS bookmarks, battery/power policy,
FreeRTOS scheduling, host tests, CI, and hardware documentation.

## MVP1.1 — flash assets and deterministic offline intelligence (implemented)

No MicroSD dependency, generated embedded asset index, NVS-only persistence, Story/Music/Knowledge
skills, replaceable `IAIEngine`, SimpleAI intent matching, offline knowledge, and flash reporting.

## MVP2 — product hardening

Brownout/recovery tests, codec corruption recovery, manifest metadata, SD hot-plug policy, calibrated
battery curve, production diagnostics, watchdogs, telemetry stored locally, and signed USB updates.

## MVP3 — offline voice

INMP441 capture service, ring buffer, wake-word and speech-recognition implementations behind the
existing interfaces, intent routing as skills, acoustic echo handling, and privacy indicators.

## MVP4 — offline AI companion

Edge LLM/TTS selection based on measured S3 constraints (or a future compute module), bounded memory,
knowledge base, safety policy, parental controls, and fully local data lifecycle. Cloud features, if
ever added, remain optional adapters with explicit consent.

## Product expansion

Create separate composition/config packages for Teni Rabbit, Dino, and Robot. Reuse core services and
interfaces; vary pins, skills, assets, and product policy without forking the platform.
