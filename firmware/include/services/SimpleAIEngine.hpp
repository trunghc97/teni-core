#pragma once

#include <memory>

#include "interfaces/AiInterfaces.hpp"
#include "interfaces/CoreInterfaces.hpp"
#include "services/IntentMatcher.hpp"

namespace teni::services {

/** Deterministic offline intent and knowledge engine with no generative model. */
class SimpleAIEngine final : public interfaces::IAIEngine {
 public:
  explicit SimpleAIEngine(interfaces::IAssetManager& assets);
  SimpleAIEngine(interfaces::IAssetManager& assets,
                 std::unique_ptr<interfaces::IIntentMatcher> matcher);
  [[nodiscard]] interfaces::AIResult process(const std::string& question) const override;

 private:
  [[nodiscard]] std::string lookupKnowledge(interfaces::Intent intent,
                                            const std::string& normalized) const;
  [[nodiscard]] static const char* topicFor(interfaces::Intent intent);
  [[nodiscard]] static const char* templateFor(interfaces::Intent intent);
  interfaces::IAssetManager& assets_;
  std::unique_ptr<interfaces::IIntentMatcher> matcher_;
};

}  // namespace teni::services
