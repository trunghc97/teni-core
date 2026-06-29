# Device state machine

```mermaid
stateDiagram-v2
  [*] --> BOOT
  BOOT --> IDLE: SystemReady
  IDLE --> PLAYING: MediaStarted
  PLAYING --> PAUSED: MediaPaused
  PAUSED --> PLAYING: MediaResumed
  PLAYING --> IDLE: MediaStopped / MediaFinished
  IDLE --> CHARGING: ChargeStarted
  PLAYING --> CHARGING: ChargeStarted
  PAUSED --> CHARGING: ChargeStarted
  CHARGING --> IDLE: ChargeStopped
  IDLE --> LOW_BATTERY: LowBattery
  PLAYING --> LOW_BATTERY: LowBattery
  IDLE --> SLEEP: SleepRequested
  PAUSED --> SLEEP: SleepRequested
  LOW_BATTERY --> POWER_OFF: CriticalBattery
  IDLE --> POWER_OFF: ShutdownRequested
  SLEEP --> [*]
  POWER_OFF --> [*]
```

Charging temporarily overlays the prior state; MVP1.1 returns to a safe idle if that state cannot be
restored. Deep sleep wakes from the play button. A physical load-switch latch is required for true
zero/near-zero shutdown; otherwise `POWER_OFF` falls back to deep sleep.
