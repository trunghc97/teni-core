#include "core/DeviceStateMachine.hpp"

namespace teni::core {

DeviceStateMachine::DeviceStateMachine(interfaces::IEventBus& bus) : bus_(bus) {}

void DeviceStateMachine::handle(const events::Event& event) {
  using events::Type;
  switch (event.type) {
    case Type::SystemReady:
      transition(DeviceState::Idle, event.timestampMs);
      break;
    case Type::MediaStarted:
    case Type::MediaResumed:
      transition(DeviceState::Playing, event.timestampMs);
      break;
    case Type::MediaPaused:
      transition(DeviceState::Paused, event.timestampMs);
      break;
    case Type::MediaStopped:
    case Type::MediaFinished:
      transition(DeviceState::Idle, event.timestampMs);
      break;
    case Type::LowBattery:
      transition(DeviceState::LowBattery, event.timestampMs);
      break;
    case Type::ChargeStarted:
      preChargeState_ = state_;
      transition(DeviceState::Charging, event.timestampMs);
      break;
    case Type::ChargeStopped:
      transition(preChargeState_ == DeviceState::Charging ? DeviceState::Idle : preChargeState_,
                 event.timestampMs);
      break;
    case Type::SleepRequested:
      transition(DeviceState::Sleep, event.timestampMs);
      break;
    case Type::ShutdownRequested:
    case Type::CriticalBattery:
      transition(DeviceState::PowerOff, event.timestampMs);
      break;
    default:
      break;
  }
}

void DeviceStateMachine::transition(DeviceState next, std::uint32_t timestamp) {
  if (next == state_) return;
  const auto previous = state_;
  state_ = next;
  bus_.publish({events::Type::StateChanged, timestamp, static_cast<std::int32_t>(previous),
                static_cast<std::int32_t>(next)});
}

const char* DeviceStateMachine::name(DeviceState state) {
  switch (state) {
    case DeviceState::Boot:
      return "BOOT";
    case DeviceState::Idle:
      return "IDLE";
    case DeviceState::Playing:
      return "PLAYING";
    case DeviceState::Paused:
      return "PAUSED";
    case DeviceState::Sleep:
      return "SLEEP";
    case DeviceState::Charging:
      return "CHARGING";
    case DeviceState::LowBattery:
      return "LOW_BATTERY";
    case DeviceState::PowerOff:
      return "POWER_OFF";
  }
  return "UNKNOWN";
}

}  // namespace teni::core
