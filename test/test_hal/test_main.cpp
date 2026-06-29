#include <unity.h>

#include "../mocks/Mocks.hpp"

void test_storage_contract_is_atomic_and_readable() {
  teni::test::MockStorage storage;
  TEST_ASSERT_TRUE(storage.begin());
  TEST_ASSERT_TRUE(storage.writeTextAtomic("/config/test.json", "{}"));
  std::string value;
  TEST_ASSERT_TRUE(storage.readText("/config/test.json", value));
  TEST_ASSERT_EQUAL_STRING("{}", value.c_str());
}

void test_audio_contract_tracks_state() {
  teni::test::MockAudio audio;
  TEST_ASSERT_TRUE(audio.play("/stories/a.mp3", 12));
  TEST_ASSERT_TRUE(audio.isPlaying());
  audio.pause();
  TEST_ASSERT_TRUE(audio.isPaused());
  audio.resume();
  TEST_ASSERT_FALSE(audio.isPaused());
  audio.stop();
  TEST_ASSERT_FALSE(audio.isPlaying());
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_storage_contract_is_atomic_and_readable);
  RUN_TEST(test_audio_contract_tracks_state);
  return UNITY_END();
}
