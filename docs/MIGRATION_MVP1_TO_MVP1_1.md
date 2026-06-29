# Migration from MVP1 to MVP1.1

## Breaking hardware/storage change

MicroSD is removed. GPIO 10–13 are no longer reserved by teni-core and the SD socket/module can be
omitted. Audio and JSON content now ship inside the signed/flashable firmware image. Existing SD card
contents are not read after upgrading.

Copy media from the old card into the matching repository folders before building:

```text
/stories/*  -> assets/stories/
/music/*    -> assets/music/
```

NVS keys remain compatible. Volume, language, current story, byte-offset bookmark, repeat, random,
shuffle, brightness, and sleep timer survive a normal firmware upgrade. MVP1.1 adds child name and
favorite story. An erase operation still clears all NVS values.

Story paths remain `/stories/name.ext`, so bookmarks continue to resolve when the embedded filename
matches the former SD filename. If a story is removed or renamed, the library safely starts at its
first available story.

## Software changes

- `SdCardStorage` was removed and `AssetManager` now implements the read-only storage contract.
- `AudioEngine` reads an `AssetView` from flash instead of opening an SD file.
- StoryPlayer/MusicPlayer became StorySkill/MusicSkill; compatibility type aliases remain.
- Bookmarks are NVS-only; the writable SD `bookmarks.json` mirror no longer exists.
- `IAIEngine` and `SimpleAIEngine` add deterministic offline intents and knowledge without an LLM.
- The app partition changed from dual 6 MiB OTA slots to one 14.875 MiB offline image. Reflash the
  partition table when migrating. Internet OTA remains out of scope.

