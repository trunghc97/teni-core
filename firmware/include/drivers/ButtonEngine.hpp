#pragma once

#include <array>
#include <cstdint>

#include "interfaces/CoreInterfaces.hpp"

namespace teni::drivers {

struct ButtonConfig final {
  events::ButtonId id;
  int pin;
  bool activeLow{true};
  std::uint16_t debounceMs{30};
  std::uint16_t longPressMs{800};
  std::uint16_t multiClickGapMs{350};
};

class ButtonEngine final {
 public:
  using ReadPin = std::function<bool(int)>;
  ButtonEngine(interfaces::IEventBus& bus, ReadPin reader);
  bool add(const ButtonConfig& config);
  void poll(std::uint32_t nowMs);

 private:
  struct Runtime final {
    ButtonConfig config{};
    bool used{false};
    bool rawPressed{false};
    bool stablePressed{false};
    bool longSent{false};
    std::uint8_t clicks{0};
    std::uint32_t rawChangedAt{0};
    std::uint32_t pressedAt{0};
    std::uint32_t releasedAt{0};
  };
  interfaces::IEventBus& bus_;
  ReadPin reader_;
  std::array<Runtime, 8> buttons_{};
};

}  // namespace teni::drivers
