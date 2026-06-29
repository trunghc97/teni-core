#include "drivers/ButtonEngine.hpp"

namespace teni::drivers {

ButtonEngine::ButtonEngine(interfaces::IEventBus& bus, ReadPin reader)
    : bus_(bus), reader_(std::move(reader)) {}

bool ButtonEngine::add(const ButtonConfig& config) {
  for (auto& button : buttons_) {
    if (!button.used) {
      button.config = config;
      button.used = true;
      return true;
    }
  }
  return false;
}

void ButtonEngine::poll(std::uint32_t nowMs) {
  for (auto& button : buttons_) {
    if (!button.used) continue;
    const bool level = reader_(button.config.pin);
    const bool pressed = button.config.activeLow ? !level : level;
    if (pressed != button.rawPressed) {
      button.rawPressed = pressed;
      button.rawChangedAt = nowMs;
    }
    if (pressed != button.stablePressed &&
        nowMs - button.rawChangedAt >= button.config.debounceMs) {
      button.stablePressed = pressed;
      if (pressed) {
        button.pressedAt = nowMs;
        button.longSent = false;
      } else if (!button.longSent) {
        ++button.clicks;
        button.releasedAt = nowMs;
      }
    }
    if (button.stablePressed && !button.longSent &&
        nowMs - button.pressedAt >= button.config.longPressMs) {
      button.longSent = true;
      button.clicks = 0;
      bus_.publish(events::Event::button(button.config.id, events::Gesture::LongPress, 1, nowMs));
    }
    if (!button.stablePressed && button.clicks > 0 &&
        nowMs - button.releasedAt >= button.config.multiClickGapMs) {
      const auto gesture = button.clicks == 1   ? events::Gesture::ShortPress
                           : button.clicks == 2 ? events::Gesture::DoubleClick
                                                : events::Gesture::MultiClick;
      bus_.publish(events::Event::button(button.config.id, gesture, button.clicks, nowMs));
      button.clicks = 0;
    }
  }
}

}  // namespace teni::drivers
