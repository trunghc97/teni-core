#pragma once

#include <cstdint>

namespace teni::events {

enum class Type : std::uint8_t {
  None,
  SystemReady,
  Tick,
  ButtonGesture,
  PlayPauseRequested,
  StopRequested,
  NextRequested,
  PreviousRequested,
  RandomRequested,
  RepeatRequested,
  VolumeChanged,
  MediaStarted,
  MediaPaused,
  MediaResumed,
  MediaStopped,
  MediaFinished,
  AudioError,
  BatteryUpdated,
  LowBattery,
  CriticalBattery,
  ChargeStarted,
  ChargeStopped,
  SleepRequested,
  ShutdownRequested,
  StateChanged
};

enum class ButtonId : std::uint8_t { Play, Next, Previous, Mode };
enum class Gesture : std::uint8_t { ShortPress, LongPress, DoubleClick, MultiClick };
enum class MediaKind : std::uint8_t { Story, Music };

struct Event final {
  Type type{Type::None};
  std::uint32_t timestampMs{0};
  std::int32_t value{0};
  std::int32_t value2{0};

  static Event button(ButtonId id, Gesture gesture, std::uint8_t clicks, std::uint32_t timestamp) {
    return {Type::ButtonGesture, timestamp, static_cast<std::int32_t>(id),
            (static_cast<std::int32_t>(gesture) << 8) | clicks};
  }
};

}  // namespace teni::events
