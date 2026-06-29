#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#include <Preferences.h>
#include <driver/rtc_io.h>
#include <esp_sleep.h>
#endif

#include <memory>

#include "interfaces/CoreInterfaces.hpp"

#ifdef ARDUINO
class Adafruit_NeoPixel;
#endif

namespace teni::drivers {

class ArduinoClock final : public interfaces::IClock {
 public:
  [[nodiscard]] std::uint32_t milliseconds() const override;
};

class NvsConfigStore final : public interfaces::IConfigStore {
 public:
  bool load(config::AppConfig& config) override;
  bool save(const config::AppConfig& config) override;
};

class Esp32Battery final : public interfaces::IBattery {
 public:
  Esp32Battery(int adcPin, int chargePin, float dividerRatio);
  void begin();
  [[nodiscard]] float voltage() const override;
  [[nodiscard]] std::uint8_t percent() const override;
  [[nodiscard]] bool charging() const override;

 private:
  int adcPin_;
  int chargePin_;
  float dividerRatio_;
};

class Esp32Power final : public interfaces::IPower {
 public:
  Esp32Power(int wakePin, int powerHoldPin) : wakePin_(wakePin), powerHoldPin_(powerHoldPin) {}
  void begin();
  void deepSleep() override;
  void powerOff() override;

 private:
  int wakePin_;
  int powerHoldPin_;
};

class Esp32RgbLed final : public interfaces::ILed {
 public:
  explicit Esp32RgbLed(int pin);
  ~Esp32RgbLed() override;
  void begin();
  void setRgb(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
              std::uint8_t brightness) override;
  void off() override;

 private:
  int pin_;
#ifdef ARDUINO
  std::unique_ptr<Adafruit_NeoPixel> pixel_;
#endif
};

}  // namespace teni::drivers
