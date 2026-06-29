#include <unity.h>

#include "../mocks/Mocks.hpp"
#include "core/EventBus.hpp"
#include "modules/story/StoryPlayer.hpp"
#include "services/MediaLibrary.hpp"

void test_story_resumes_and_advances() {
  teni::test::MockStorage storage;
  storage.files["/stories.mp3"] = {"/stories/story001.mp3", "/stories/story002.mp3"};
  teni::services::MediaLibrary library(storage);
  teni::test::MockAudio audio;
  teni::test::MockConfigStore store;
  teni::config::AppConfig config;
  config.currentStory = "/stories/story001.mp3";
  config.playbackPosition = 1234;
  teni::modules::story::StoryPlayer player(library, audio, store, config);
  TEST_ASSERT_TRUE(player.initialize());
  TEST_ASSERT_TRUE(player.play());
  TEST_ASSERT_EQUAL_UINT32(1234, audio.lastOffset);
  TEST_ASSERT_TRUE(player.next());
  TEST_ASSERT_EQUAL_STRING("/stories/story002.mp3", audio.lastPath.c_str());
  TEST_ASSERT_EQUAL_UINT32(0, audio.lastOffset);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_story_resumes_and_advances);
  return UNITY_END();
}
