#include "hal/FreeRtosPrimitives.hpp"

namespace teni::hal {
EventGroup::EventGroup() {
#ifdef ARDUINO
  handle_ = xEventGroupCreate();
#endif
}
EventGroup::~EventGroup() {
#ifdef ARDUINO
  if (handle_ != nullptr) vEventGroupDelete(handle_);
#endif
}
void EventGroup::set(std::uint32_t bits) {
#ifdef ARDUINO
  if (handle_ != nullptr) xEventGroupSetBits(handle_, bits);
#else
  bits_ |= bits;
#endif
}
bool EventGroup::has(std::uint32_t bits) const {
#ifdef ARDUINO
  return handle_ != nullptr && (xEventGroupGetBits(handle_) & bits) == bits;
#else
  return (bits_ & bits) == bits;
#endif
}

PeriodicTimer::PeriodicTimer(const char* name, std::uint32_t periodMs,
                             std::function<void()> callback)
    : callback_(std::move(callback)) {
#ifdef ARDUINO
  handle_ = xTimerCreate(name, pdMS_TO_TICKS(periodMs), pdTRUE, this, invoke);
#else
  (void)name;
  (void)periodMs;
#endif
}
PeriodicTimer::~PeriodicTimer() {
#ifdef ARDUINO
  if (handle_ != nullptr) {
    xTimerStop(handle_, 0);
    xTimerDelete(handle_, 0);
  }
#endif
}
bool PeriodicTimer::start() {
#ifdef ARDUINO
  return handle_ != nullptr && xTimerStart(handle_, 0) == pdPASS;
#else
  return false;
#endif
}
#ifdef ARDUINO
void PeriodicTimer::invoke(TimerHandle_t timer) {
  auto* self = static_cast<PeriodicTimer*>(pvTimerGetTimerID(timer));
  if (self != nullptr && self->callback_) self->callback_();
}
#endif
}  // namespace teni::hal
