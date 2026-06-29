#include <unity.h>

#include "core/DeviceStateMachine.hpp"
#include "core/EventBus.hpp"

void test_event_bus_dispatches_by_type() {
  teni::core::EventBus bus(4);
  int calls = 0;
  bus.subscribe(teni::events::Type::SystemReady, [&](const auto&) { ++calls; });
  TEST_ASSERT_TRUE(bus.publish({teni::events::Type::SystemReady, 42}));
  bus.dispatchPending();
  TEST_ASSERT_EQUAL_INT(1, calls);
}

void test_state_machine_tracks_playback() {
  teni::core::EventBus bus;
  teni::core::DeviceStateMachine state(bus);
  state.handle({teni::events::Type::SystemReady, 1});
  TEST_ASSERT_EQUAL_INT(static_cast<int>(teni::core::DeviceState::Idle),
                        static_cast<int>(state.state()));
  state.handle({teni::events::Type::MediaStarted, 2});
  TEST_ASSERT_EQUAL_INT(static_cast<int>(teni::core::DeviceState::Playing),
                        static_cast<int>(state.state()));
  state.handle({teni::events::Type::MediaPaused, 3});
  TEST_ASSERT_EQUAL_INT(static_cast<int>(teni::core::DeviceState::Paused),
                        static_cast<int>(state.state()));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_event_bus_dispatches_by_type);
  RUN_TEST(test_state_machine_tracks_playback);
  return UNITY_END();
}
