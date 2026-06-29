#include "drivers/Esp32Drivers.hpp"

#include <algorithm>

#ifdef ARDUINO
#include <Adafruit_NeoPixel.h>
#endif

namespace teni::drivers {

std::uint32_t ArduinoClock::milliseconds() const {
#ifdef ARDUINO
  return millis();
#else
  return 0;
#endif
}

bool NvsConfigStore::load(config::AppConfig& config) {
#ifdef ARDUINO
  Preferences nvs;
  if (!nvs.begin("teni", true)) return false;
  config.volume = nvs.getUChar("volume", config.volume);
  config.brightness = nvs.getUChar("brightness", config.brightness);
  config.sleepTimerMinutes = nvs.getUInt("sleep_min", config.sleepTimerMinutes);
  config.language = nvs.getString("language", config.language.c_str()).c_str();
  config.currentStory = nvs.getString("story", config.currentStory.c_str()).c_str();
  config.childName = nvs.getString("child", config.childName.c_str()).c_str();
  config.favoriteStory = nvs.getString("favorite", config.favoriteStory.c_str()).c_str();
  config.playbackPosition = nvs.getUInt("position", config.playbackPosition);
  config.repeatStory = nvs.getBool("repeat", config.repeatStory);
  config.storyRandom = nvs.getBool("random", config.storyRandom);
  config.musicShuffle = nvs.getBool("shuffle", config.musicShuffle);
  nvs.end();
  return true;
#else
  (void)config;
  return false;
#endif
}

bool NvsConfigStore::save(const config::AppConfig& config) {
#ifdef ARDUINO
  Preferences nvs;
  if (!nvs.begin("teni", false)) return false;
  bool ok = true;
  ok &= nvs.putUChar("volume", config.volume) > 0;
  ok &= nvs.putUChar("brightness", config.brightness) > 0;
  ok &= nvs.putUInt("sleep_min", config.sleepTimerMinutes) > 0;
  ok &= nvs.putString("language", config.language.c_str()) > 0;
  ok &= nvs.putString("story", config.currentStory.c_str()) > 0 || config.currentStory.empty();
  ok &= nvs.putString("child", config.childName.c_str()) > 0 || config.childName.empty();
  ok &= nvs.putString("favorite", config.favoriteStory.c_str()) > 0 || config.favoriteStory.empty();
  ok &= nvs.putUInt("position", config.playbackPosition) > 0 || config.playbackPosition == 0;
  ok &= nvs.putBool("repeat", config.repeatStory) > 0;
  ok &= nvs.putBool("random", config.storyRandom) > 0;
  ok &= nvs.putBool("shuffle", config.musicShuffle) > 0;
  nvs.end();
  return ok;
#else
  (void)config;
  return false;
#endif
}

Esp32Battery::Esp32Battery(int adcPin, int chargePin, float dividerRatio)
    : adcPin_(adcPin), chargePin_(chargePin), dividerRatio_(dividerRatio) {}
void Esp32Battery::begin() {
#ifdef ARDUINO
  pinMode(adcPin_, INPUT);
  pinMode(chargePin_, INPUT_PULLUP);
  analogSetPinAttenuation(adcPin_, ADC_11db);
#endif
}
float Esp32Battery::voltage() const {
#ifdef ARDUINO
  return static_cast<float>(analogReadMilliVolts(adcPin_)) * dividerRatio_ / 1000.0F;
#else
  return 0.0F;
#endif
}
std::uint8_t Esp32Battery::percent() const {
  const float normalized = (voltage() - 3.2F) / (4.2F - 3.2F);
  return static_cast<std::uint8_t>(std::clamp(normalized * 100.0F, 0.0F, 100.0F));
}
bool Esp32Battery::charging() const {
#ifdef ARDUINO
  return digitalRead(chargePin_) == LOW;
#else
  return false;
#endif
}

void Esp32Power::begin() {
#ifdef ARDUINO
  pinMode(powerHoldPin_, OUTPUT);
  digitalWrite(powerHoldPin_, HIGH);
#endif
}
void Esp32Power::deepSleep() {
#ifdef ARDUINO
  esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(wakePin_), 0);
  esp_deep_sleep_start();
#endif
}
void Esp32Power::powerOff() {
#ifdef ARDUINO
  digitalWrite(powerHoldPin_, LOW);
  deepSleep();
#endif
}

Esp32RgbLed::Esp32RgbLed(int pin) : pin_(pin) {}
Esp32RgbLed::~Esp32RgbLed() = default;
void Esp32RgbLed::begin() {
#ifdef ARDUINO
  pixel_ = std::make_unique<Adafruit_NeoPixel>(1, pin_, NEO_GRB + NEO_KHZ800);
  pixel_->begin();
  pixel_->clear();
  pixel_->show();
#endif
}
void Esp32RgbLed::setRgb(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
                         std::uint8_t brightness) {
#ifdef ARDUINO
  if (!pixel_) return;
  pixel_->setBrightness(brightness);
  pixel_->setPixelColor(0, pixel_->Color(red, green, blue));
  pixel_->show();
#else
  (void)red;
  (void)green;
  (void)blue;
  (void)brightness;
#endif
}
void Esp32RgbLed::off() { setRgb(0, 0, 0, 0); }

}  // namespace teni::drivers
