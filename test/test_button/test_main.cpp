#include <unity.h>

#include "core/EventBus.hpp"
#include "drivers/ButtonEngine.hpp"

void test_button_emits_double_click() {
  teni::core::EventBus bus;
  bool level = true;
  teni::drivers::ButtonEngine buttons(bus, [&](int) { return level; });
  buttons.add({teni::events::ButtonId::Play, 1, true, 20, 800, 100});
  int gesture = -1;
  bus.subscribe(teni::events::Type::ButtonGesture,
                [&](const auto& event) { gesture = (event.value2 >> 8) & 0xFF; });
  level = false;
  buttons.poll(0);
  buttons.poll(25);
  level = true;
  buttons.poll(30);
  buttons.poll(55);
  level = false;
  buttons.poll(60);
  buttons.poll(85);
  level = true;
  buttons.poll(90);
  buttons.poll(115);
  buttons.poll(220);
  bus.dispatchPending();
  TEST_ASSERT_EQUAL_INT(static_cast<int>(teni::events::Gesture::DoubleClick), gesture);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_button_emits_double_click);
  return UNITY_END();
}
