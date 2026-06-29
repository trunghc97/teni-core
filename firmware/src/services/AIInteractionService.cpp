#include "services/AIInteractionService.hpp"

namespace teni::services {

bool AIInteractionService::isKnowledgeIntent(interfaces::Intent intent) {
  using interfaces::Intent;
  return intent == Intent::Greeting || intent == Intent::Animal || intent == Intent::Color ||
         intent == Intent::Alphabet || intent == Intent::Number || intent == Intent::Math ||
         intent == Intent::Bedtime || intent == Intent::Unknown;
}

interfaces::AIResult AIInteractionService::process(const std::string& question) {
  auto result = ai_.process(question);
  playback_.handleIntent(result.intent);
  if (isKnowledgeIntent(result.intent) && !result.responseAudioAsset.empty()) {
    audio_.play(result.responseAudioAsset);
  }
  return result;
}

}  // namespace teni::services
