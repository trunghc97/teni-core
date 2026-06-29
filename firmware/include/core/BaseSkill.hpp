#pragma once

#include "interfaces/CoreInterfaces.hpp"

namespace teni::core {

/** Small lifecycle base for statically linked product skills. */
class BaseSkill : public interfaces::ISkill {
 public:
  explicit BaseSkill(const char* skillName) : skillName_(skillName) {}
  [[nodiscard]] const char* name() const override { return skillName_; }
  bool initialize() override {
    initialized_ = true;
    return true;
  }
  void shutdown() override { initialized_ = false; }
  [[nodiscard]] bool initialized() const { return initialized_; }

 protected:
  void markInitialized(bool value) { initialized_ = value; }

 private:
  const char* skillName_;
  bool initialized_{false};
};

}  // namespace teni::core
