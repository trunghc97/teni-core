#include "services/AIEngineFactory.hpp"

#include "services/SimpleAIEngine.hpp"

namespace teni::services {

std::unique_ptr<interfaces::IAIEngine> createAIEngine(interfaces::IAssetManager& assets) {
  return std::make_unique<SimpleAIEngine>(assets);
}

}  // namespace teni::services
