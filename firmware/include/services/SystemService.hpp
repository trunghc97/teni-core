#pragma once

#include "config/AppConfig.hpp"
#include "core/DeviceStateMachine.hpp"
#include "interfaces/CoreInterfaces.hpp"

namespace teni::services {
class SystemService final {
 public:
  SystemService(interfaces::IEventBus& bus, interfaces::IClock& clock,
                interfaces::IBattery& battery, interfaces::ILed& led, interfaces::IPower& power,
                config::AppConfig& config);
  void handle(const events::Event& event);
  void tick();

 private:
  interfaces::IEventBus& bus_;
  interfaces::IClock& clock_;
  interfaces::IBattery& battery_;
  interfaces::ILed& led_;
  interfaces::IPower& power_;
  config::AppConfig& config_;
  std::uint32_t lastActivity_{0};
  std::uint32_t lastBatteryPoll_{0};
  bool wasCharging_{false};
};
}  // namespace teni::services
