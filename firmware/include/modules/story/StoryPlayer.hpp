#pragma once

#include <cstddef>
#include <cstdint>

#include "config/AppConfig.hpp"
#include "interfaces/CoreInterfaces.hpp"
#include "services/MediaLibrary.hpp"

namespace teni::modules::story {
class StorySkill final : public interfaces::ISkill {
 public:
  StorySkill(services::MediaLibrary& library, interfaces::IAudioEngine& audio,
             interfaces::IConfigStore& store, config::AppConfig& config);
  [[nodiscard]] const char* name() const override { return "story"; }
  bool initialize() override;
  void shutdown() override;
  bool play();
  void togglePause();
  void pause();
  void resume();
  void stop();
  bool next();
  bool previous();
  bool random();
  void toggleRepeat();
  void finished();
  void saveBookmark();

 private:
  bool playCurrent(std::uint32_t offset = 0);
  services::MediaLibrary& library_;
  interfaces::IAudioEngine& audio_;
  interfaces::IConfigStore& store_;
  config::AppConfig& config_;
  std::size_t index_{0};
  std::uint32_t randomState_{0x54454E49U};
};
using StoryPlayer = StorySkill;
}  // namespace teni::modules::story
