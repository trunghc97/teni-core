#pragma once

#include <cstdint>
#include <string>

namespace teni::config {

struct AppConfig final {
  std::uint8_t volume{65};
  std::uint8_t brightness{40};
  std::uint32_t sleepTimerMinutes{30};
  std::string language{"en"};
  std::string currentStory{};
  std::string childName{};
  std::string favoriteStory{};
  std::uint32_t playbackPosition{0};
  bool repeatStory{false};
  bool storyRandom{false};
  bool musicShuffle{false};
};

}  // namespace teni::config
