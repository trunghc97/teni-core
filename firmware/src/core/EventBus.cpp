#include "core/EventBus.hpp"

#include "core/Logger.hpp"

namespace teni::core {

EventBus::EventBus(std::size_t capacity)
#ifdef ARDUINO
    : queue_(xQueueCreate(static_cast<UBaseType_t>(capacity), sizeof(events::Event)))
#else
    : capacity_(capacity)
#endif
{
}

EventBus::~EventBus() {
#ifdef ARDUINO
  if (queue_ != nullptr) vQueueDelete(queue_);
#endif
}

bool EventBus::publish(const events::Event& event) {
#ifdef ARDUINO
  return queue_ != nullptr && xQueueSend(queue_, &event, 0) == pdTRUE;
#else
  std::lock_guard<std::mutex> lock(mutex_);
  if (queue_.size() >= capacity_) return false;
  queue_.push_back(event);
  return true;
#endif
}

void EventBus::subscribe(events::Type type, Handler handler) {
  subscriptions_.push_back({type, std::move(handler)});
}

void EventBus::dispatchPending(std::size_t maximum) {
  for (std::size_t count = 0; count < maximum; ++count) {
    events::Event event;
#ifdef ARDUINO
    if (queue_ == nullptr || xQueueReceive(queue_, &event, 0) != pdTRUE) break;
#else
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (queue_.empty()) break;
      event = queue_.front();
      queue_.pop_front();
    }
#endif
    for (const auto& subscription : subscriptions_) {
      if (subscription.type == event.type) subscription.handler(event);
    }
  }
}

}  // namespace teni::core
