#pragma once

#include "interfaces/CoreInterfaces.hpp"

namespace teni::core {

enum class DeviceState : std::uint8_t {
  Boot,
  Idle,
  Playing,
  Paused,
  Sleep,
  Charging,
  LowBattery,
  PowerOff
};

class DeviceStateMachine final {
 public:
  explicit DeviceStateMachine(interfaces::IEventBus& bus);
  void handle(const events::Event& event);
  [[nodiscard]] DeviceState state() const { return state_; }
  [[nodiscard]] static const char* name(DeviceState state);

 private:
  void transition(DeviceState next, std::uint32_t timestamp);
  interfaces::IEventBus& bus_;
  DeviceState state_{DeviceState::Boot};
  DeviceState preChargeState_{DeviceState::Idle};
};

}  // namespace teni::core
