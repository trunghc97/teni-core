#ifdef ARDUINO
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "app/TeniApplication.hpp"
#include "core/Logger.hpp"

namespace {
void applicationTask(void*) {
  teni::app::TeniApplication application;
  if (!application.initialize()) {
    LOG_E("Main", "Initialization failed; entering safe idle");
    vTaskDelete(nullptr);
  }
  application.run();
}
}  // namespace

void setup() {
  Serial.begin(115200);
  xTaskCreatePinnedToCore(applicationTask, "teni-app", 12288, nullptr, 3, nullptr, 1);
}

void loop() { vTaskDelete(nullptr); }
#endif
