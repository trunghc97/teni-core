#pragma once

#include "interfaces/AiInterfaces.hpp"
#include "interfaces/CoreInterfaces.hpp"
#include "services/PlaybackCoordinator.hpp"

namespace teni::services {

/** Executes AI intents and optionally plays pre-recorded knowledge responses. */
class AIInteractionService final {
 public:
  AIInteractionService(interfaces::IAIEngine& ai, PlaybackCoordinator& playback,
                       interfaces::IAudioEngine& audio)
      : ai_(ai), playback_(playback), audio_(audio) {}
  [[nodiscard]] interfaces::AIResult process(const std::string& question);

 private:
  [[nodiscard]] static bool isKnowledgeIntent(interfaces::Intent intent);
  interfaces::IAIEngine& ai_;
  PlaybackCoordinator& playback_;
  interfaces::IAudioEngine& audio_;
};

}  // namespace teni::services
