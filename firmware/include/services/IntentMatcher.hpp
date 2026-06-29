#pragma once

#include <string>
#include <vector>

#include "interfaces/AiInterfaces.hpp"

namespace teni::services {

/** Priority-based phrase, keyword, and bounded fuzzy intent matcher. */
class IntentMatcher final : public interfaces::IIntentMatcher {
 public:
  struct Rule final {
    interfaces::Intent intent;
    int priority;
    std::vector<const char*> patterns;
    std::vector<const char*> keywords;
  };
  [[nodiscard]] interfaces::Intent match(const std::string& question,
                                         float& confidence) const override;
  [[nodiscard]] static std::string normalize(const std::string& text);

 private:
  [[nodiscard]] static std::size_t editDistance(const std::string& left, const std::string& right);
};

}  // namespace teni::services
