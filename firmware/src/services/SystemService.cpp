#include "services/SystemService.hpp"

#include "config/BoardConfig.hpp"

namespace teni::services {
SystemService::SystemService(interfaces::IEventBus& bus, interfaces::IClock& clock,
                             interfaces::IBattery& battery, interfaces::ILed& led,
                             interfaces::IPower& power, config::AppConfig& config)
    : bus_(bus), clock_(clock), battery_(battery), led_(led), power_(power), config_(config) {}
void SystemService::handle(const events::Event& event) {
  if (event.type == events::Type::ButtonGesture || event.type == events::Type::MediaStarted ||
      event.type == events::Type::MediaResumed)
    lastActivity_ = event.timestampMs;
  if (event.type == events::Type::StateChanged) {
    const auto state = static_cast<core::DeviceState>(event.value2);
    switch (state) {
      case core::DeviceState::Boot:
        led_.setRgb(0, 0, 255, config_.brightness);
        break;
      case core::DeviceState::Idle:
        led_.setRgb(0, 40, 0, config_.brightness);
        break;
      case core::DeviceState::Playing:
        led_.setRgb(0, 0, 120, config_.brightness);
        break;
      case core::DeviceState::Paused:
        led_.setRgb(80, 40, 0, config_.brightness);
        break;
      case core::DeviceState::Charging:
        led_.setRgb(0, 80, 0, config_.brightness);
        break;
      case core::DeviceState::LowBattery:
        led_.setRgb(150, 20, 0, config_.brightness);
        break;
      case core::DeviceState::Sleep:
        led_.off();
        power_.deepSleep();
        break;
      case core::DeviceState::PowerOff:
        led_.off();
        power_.powerOff();
        break;
    }
  }
}
void SystemService::tick() {
  const auto now = clock_.milliseconds();
  if (now - lastBatteryPoll_ >= 5000U) {
    lastBatteryPoll_ = now;
    const auto volts = battery_.voltage();
    bus_.publish({events::Type::BatteryUpdated, now, battery_.percent(),
                  static_cast<std::int32_t>(volts * 1000.0F)});
    if (volts <= config::HardwareLimits::kCriticalBatteryVolts)
      bus_.publish({events::Type::CriticalBattery, now});
    else if (volts <= config::HardwareLimits::kLowBatteryVolts)
      bus_.publish({events::Type::LowBattery, now});
    const bool charging = battery_.charging();
    if (charging != wasCharging_) {
      wasCharging_ = charging;
      bus_.publish({charging ? events::Type::ChargeStarted : events::Type::ChargeStopped, now});
    }
  }
  const auto timeout = config_.sleepTimerMinutes * 60000U;
  if (timeout > 0 && now - lastActivity_ >= timeout)
    bus_.publish({events::Type::SleepRequested, now});
}
}  // namespace teni::services
