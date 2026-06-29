#pragma once

#include "core/BaseSkill.hpp"

namespace teni::modules::placeholders {

class LearningSkill final : public core::BaseSkill {
 public:
  LearningSkill() : BaseSkill("learning") {}
};
class ConversationSkill final : public core::BaseSkill {
 public:
  ConversationSkill() : BaseSkill("conversation") {}
};
class MemorySkill final : public core::BaseSkill {
 public:
  MemorySkill() : BaseSkill("memory") {}
};
class LLMSkill final : public core::BaseSkill {
 public:
  LLMSkill() : BaseSkill("llm") {}
};

}  // namespace teni::modules::placeholders
