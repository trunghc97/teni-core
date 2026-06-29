#pragma once

#include "core/BaseSkill.hpp"
#include "services/AIInteractionService.hpp"

namespace teni::modules::knowledge {

class KnowledgeSkill final : public core::BaseSkill {
 public:
  explicit KnowledgeSkill(services::AIInteractionService& interaction)
      : BaseSkill("knowledge"), interaction_(interaction) {}
  [[nodiscard]] interfaces::AIResult answer(const std::string& question) {
    return interaction_.process(question);
  }

 private:
  services::AIInteractionService& interaction_;
};

}  // namespace teni::modules::knowledge
