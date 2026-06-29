#include "services/AssetManager.hpp"

#include <algorithm>
#include <cstring>

#include "generated/assets.h"

namespace teni::services {

bool AssetManager::begin() { return true; }

bool AssetManager::registerAsset(const std::string& id, const std::string& path,
                                 const std::string& mime, const std::uint8_t* data,
                                 std::size_t size) {
  if (id.empty() || path.empty() || data == nullptr || size == 0) return false;
  registeredAssets_.push_back({id, path, mime, data, size});
  return true;
}

bool AssetManager::findAsset(const std::string& idOrPath, interfaces::AssetView& view) const {
  for (auto iterator = registeredAssets_.rbegin(); iterator != registeredAssets_.rend();
       ++iterator) {
    if (idOrPath == iterator->id || idOrPath == iterator->path) {
      view = {iterator->data, iterator->size, iterator->mime};
      return true;
    }
  }
  for (std::size_t index = 0; index < generated::kAssetCount; ++index) {
    const auto& asset = generated::kAssets[index];
    if (idOrPath == asset.id || idOrPath == asset.path) {
      view = {asset.data, asset.size, asset.mime};
      return true;
    }
  }
  view = {};
  return false;
}

bool AssetManager::exists(const std::string& path) const {
  interfaces::AssetView view;
  return findAsset(path, view);
}

std::vector<std::string> AssetManager::list(const std::string& directory,
                                            const std::string& extension) const {
  std::vector<std::string> result;
  const std::string prefix = directory.back() == '/' ? directory : directory + "/";
  for (std::size_t index = 0; index < generated::kAssetCount; ++index) {
    const std::string path = generated::kAssets[index].path;
    if (path.rfind(prefix, 0) == 0 && path.size() >= extension.size() &&
        path.compare(path.size() - extension.size(), extension.size(), extension) == 0) {
      result.push_back(path);
    }
  }
  for (const auto& asset : registeredAssets_) {
    if (asset.path.rfind(prefix, 0) == 0 && asset.path.size() >= extension.size() &&
        asset.path.compare(asset.path.size() - extension.size(), extension.size(), extension) ==
            0) {
      result.push_back(asset.path);
    }
  }
  std::sort(result.begin(), result.end());
  result.erase(std::unique(result.begin(), result.end()), result.end());
  return result;
}

bool AssetManager::readText(const std::string& path, std::string& output) const {
  interfaces::AssetView view;
  if (!findAsset(path, view)) return false;
  output.assign(reinterpret_cast<const char*>(view.data), view.size);
  return true;
}

bool AssetManager::writeTextAtomic(const std::string&, const std::string&) { return false; }

std::string AssetManager::knowledge(const std::string& topic, const std::string& key) const {
  for (std::size_t index = 0; index < generated::kKnowledgeCount; ++index) {
    const auto& record = generated::kKnowledge[index];
    if (topic == record.topic && key == record.key) return record.value;
  }
  return {};
}

std::vector<std::string> AssetManager::knowledgeKeys(const std::string& topic) const {
  std::vector<std::string> result;
  for (std::size_t index = 0; index < generated::kKnowledgeCount; ++index) {
    const auto& record = generated::kKnowledge[index];
    if (topic == record.topic) result.emplace_back(record.key);
  }
  return result;
}

std::string AssetManager::responseTemplate(const std::string& key) const {
  for (std::size_t index = 0; index < generated::kTemplateCount; ++index) {
    const auto& record = generated::kTemplates[index];
    if (key == record.key) return record.value;
  }
  return {};
}

std::size_t AssetManager::embeddedBytes() const { return generated::kEmbeddedAssetBytes; }

}  // namespace teni::services
