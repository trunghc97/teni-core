#include <unity.h>

#include "interfaces/AiInterfaces.hpp"
#include "interfaces/FutureAiInterfaces.hpp"
#include "services/AssetManager.hpp"
#include "services/SimpleAIEngine.hpp"

void test_embedded_asset_manager_exposes_knowledge() {
  teni::services::AssetManager assets;
  TEST_ASSERT_TRUE(assets.begin());
  TEST_ASSERT_GREATER_THAN(0, assets.embeddedBytes());
  TEST_ASSERT_EQUAL_STRING("Con mèo kêu meo meo.", assets.knowledge("animals", "mèo").c_str());
  static const std::uint8_t pack[] = {1, 2, 3};
  TEST_ASSERT_TRUE(
      assets.registerAsset("sfx/new", "/sfx/new.wav", "audio/wav", pack, sizeof(pack)));
  teni::interfaces::AssetView view;
  TEST_ASSERT_TRUE(assets.findAsset("sfx/new", view));
  TEST_ASSERT_EQUAL_UINT32(sizeof(pack), view.size);
}

void test_simple_ai_matches_control_and_knowledge_intents() {
  teni::services::AssetManager assets;
  teni::services::SimpleAIEngine ai(assets);
  const auto control = ai.process("Teni ơi, phát nhạc nhé");
  TEST_ASSERT_EQUAL_INT(static_cast<int>(teni::interfaces::Intent::PlayMusic),
                        static_cast<int>(control.intent));
  const auto knowledge = ai.process("Con mèo kêu như thế nào?");
  TEST_ASSERT_EQUAL_INT(static_cast<int>(teni::interfaces::Intent::Animal),
                        static_cast<int>(knowledge.intent));
  TEST_ASSERT_EQUAL_STRING("Con mèo kêu meo meo.", knowledge.response.c_str());
}

void test_simple_ai_supports_fuzzy_and_math_matching() {
  teni::services::AssetManager assets;
  teni::services::SimpleAIEngine ai(assets);
  const auto fuzzy = ai.process("play musix");
  TEST_ASSERT_EQUAL_INT(static_cast<int>(teni::interfaces::Intent::PlayMusic),
                        static_cast<int>(fuzzy.intent));
  const auto math = ai.process("2 cộng 3 bằng bao nhiêu");
  TEST_ASSERT_EQUAL_INT(static_cast<int>(teni::interfaces::Intent::Math),
                        static_cast<int>(math.intent));
  TEST_ASSERT_EQUAL_STRING("2 cộng 3 bằng 5.", math.response.c_str());
  const auto unknown = ai.process("child");
  TEST_ASSERT_EQUAL_INT(static_cast<int>(teni::interfaces::Intent::Unknown),
                        static_cast<int>(unknown.intent));
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_embedded_asset_manager_exposes_knowledge);
  RUN_TEST(test_simple_ai_matches_control_and_knowledge_intents);
  RUN_TEST(test_simple_ai_supports_fuzzy_and_math_matching);
  return UNITY_END();
}
