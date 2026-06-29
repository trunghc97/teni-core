#pragma once

#include <memory>
#include <vector>

#include "interfaces/CoreInterfaces.hpp"

namespace teni::core {
class SkillRegistry final {
 public:
  template <typename Skill, typename... Args>
  Skill& add(Args&&... args) {
    auto skill = std::make_unique<Skill>(std::forward<Args>(args)...);
    auto& reference = *skill;
    skills_.push_back(std::move(skill));
    return reference;
  }
  bool initializeAll() {
    for (auto& skill : skills_)
      if (!skill->initialize()) return false;
    return true;
  }
  ~SkillRegistry() {
    for (auto it = skills_.rbegin(); it != skills_.rend(); ++it) (*it)->shutdown();
  }

 private:
  std::vector<std::unique_ptr<interfaces::ISkill>> skills_;
};
}  // namespace teni::core
