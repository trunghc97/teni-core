#pragma once

#include <cstdint>
#include <functional>

#ifdef ARDUINO
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/timers.h>
#endif

namespace teni::hal {

/** RAII owner for application lifecycle event bits. */
class EventGroup final {
 public:
  EventGroup();
  ~EventGroup();
  EventGroup(const EventGroup&) = delete;
  EventGroup& operator=(const EventGroup&) = delete;
  void set(std::uint32_t bits);
  [[nodiscard]] bool has(std::uint32_t bits) const;

 private:
#ifdef ARDUINO
  EventGroupHandle_t handle_{nullptr};
#else
  std::uint32_t bits_{0};
#endif
};

/** Non-blocking periodic FreeRTOS software timer. */
class PeriodicTimer final {
 public:
  PeriodicTimer(const char* name, std::uint32_t periodMs, std::function<void()> callback);
  ~PeriodicTimer();
  PeriodicTimer(const PeriodicTimer&) = delete;
  PeriodicTimer& operator=(const PeriodicTimer&) = delete;
  bool start();

 private:
#ifdef ARDUINO
  static void invoke(TimerHandle_t timer);
  TimerHandle_t handle_{nullptr};
#endif
  std::function<void()> callback_;
};

}  // namespace teni::hal
