#pragma once

#include "interfaces/CoreInterfaces.hpp"

namespace teni::services {

/** Registry and zero-copy access layer for assets linked into application flash. */
class AssetManager final : public interfaces::IAssetManager {
 public:
  bool begin() override;
  bool registerAsset(const std::string& id, const std::string& path, const std::string& mime,
                     const std::uint8_t* data, std::size_t size) override;
  [[nodiscard]] bool exists(const std::string& path) const override;
  [[nodiscard]] std::vector<std::string> list(const std::string& directory,
                                              const std::string& extension) const override;
  bool readText(const std::string& path, std::string& output) const override;
  bool writeTextAtomic(const std::string& path, const std::string& content) override;
  [[nodiscard]] bool findAsset(const std::string& idOrPath,
                               interfaces::AssetView& view) const override;
  [[nodiscard]] std::string knowledge(const std::string& topic,
                                      const std::string& key) const override;
  [[nodiscard]] std::vector<std::string> knowledgeKeys(const std::string& topic) const override;
  [[nodiscard]] std::string responseTemplate(const std::string& key) const override;
  [[nodiscard]] std::size_t embeddedBytes() const override;

 private:
  struct RegisteredAsset final {
    std::string id;
    std::string path;
    std::string mime;
    const std::uint8_t* data;
    std::size_t size;
  };
  std::vector<RegisteredAsset> registeredAssets_;
};

}  // namespace teni::services
