#pragma once

#include <array>
#include <cstddef>
#include <utility>
#include <vector>

#include "interfaces/CoreInterfaces.hpp"

#ifdef ARDUINO
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#else
#include <deque>
#include <mutex>
#endif

namespace teni::core {

class EventBus final : public interfaces::IEventBus {
 public:
  explicit EventBus(std::size_t capacity = 32);
  ~EventBus() override;
  EventBus(const EventBus&) = delete;
  EventBus& operator=(const EventBus&) = delete;

  bool publish(const events::Event& event) override;
  void subscribe(events::Type type, Handler handler) override;
  void dispatchPending(std::size_t maximum = 16) override;

 private:
  struct Subscription final {
    events::Type type;
    Handler handler;
  };
  std::vector<Subscription> subscriptions_;
#ifdef ARDUINO
  QueueHandle_t queue_{nullptr};
#else
  std::size_t capacity_;
  std::deque<events::Event> queue_;
  std::mutex mutex_;
#endif
};

}  // namespace teni::core
