# Playback sequence

```mermaid
sequenceDiagram
  participant GPIO as Button Engine
  participant Bus as Event Bus
  participant PC as Playback Coordinator
  participant Story as Story Skill
  participant Audio as Audio Engine
  participant Flash as Embedded Flash
  GPIO->>Bus: ButtonGesture(Play, ShortPress)
  Bus->>PC: dispatch event
  PC->>Story: togglePause/play
  Story->>Audio: play(path, bookmark offset)
  Audio->>Flash: zero-copy view and seek
  Audio->>Bus: MediaStarted
  loop Every application tick
    Audio->>Audio: decode one buffer and write I2S
  end
  Audio->>Bus: MediaFinished
  Bus->>PC: completion
  PC->>Story: next/repeat/random policy
```

Bookmark writes are rate-limited to 30 seconds during playback and also occur at stop/shutdown.
NVS is the only writable persistent store; embedded assets are immutable until the next USB flash.
