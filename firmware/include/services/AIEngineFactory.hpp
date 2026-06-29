#pragma once

#include <memory>

#include "interfaces/AiInterfaces.hpp"
#include "interfaces/CoreInterfaces.hpp"

namespace teni::services {

/** Composition boundary for replacing SimpleAIEngine with a future local implementation. */
[[nodiscard]] std::unique_ptr<interfaces::IAIEngine> createAIEngine(
    interfaces::IAssetManager& assets);

}  // namespace teni::services
