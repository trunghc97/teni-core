#pragma once

#include <cstdint>

namespace teni::config {

struct BoardPins final {
  static constexpr int kI2sBclk = 4;
  static constexpr int kI2sLrclk = 5;
  static constexpr int kI2sDout = 6;
  static constexpr int kMicData = 7;
  static constexpr int kRgbLed = 48;
  static constexpr int kBatteryAdc = 1;
  static constexpr int kChargeDetect = 2;
  static constexpr int kButtonPlay = 14;
  static constexpr int kButtonNext = 15;
  static constexpr int kButtonPrevious = 16;
  static constexpr int kButtonMode = 17;
  static constexpr int kPowerHold = 18;
};

struct HardwareLimits final {
  static constexpr std::uint32_t kButtonPollMs = 10;
  static constexpr std::uint32_t kBookmarkPeriodMs = 30000;
  static constexpr float kBatteryDividerRatio = 2.0F;
  static constexpr float kLowBatteryVolts = 3.35F;
  static constexpr float kCriticalBatteryVolts = 3.15F;
};

}  // namespace teni::config
