#pragma once

#include "interfaces/AiInterfaces.hpp"
#include "interfaces/CoreInterfaces.hpp"
#include "modules/music/MusicPlayer.hpp"
#include "modules/story/StoryPlayer.hpp"

namespace teni::services {
class PlaybackCoordinator final {
 public:
  enum class Mode : std::uint8_t { Story, Music };
  PlaybackCoordinator(interfaces::IEventBus& bus, modules::story::StorySkill& story,
                      modules::music::MusicSkill& music);
  void handleButton(const events::Event& event);
  void handleFinished(const events::Event& event);
  void handleIntent(interfaces::Intent intent);
  [[nodiscard]] Mode mode() const { return mode_; }

 private:
  interfaces::IEventBus& bus_;
  modules::story::StorySkill& story_;
  modules::music::MusicSkill& music_;
  Mode mode_{Mode::Story};
};
}  // namespace teni::services
