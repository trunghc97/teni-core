#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "interfaces/CoreInterfaces.hpp"

namespace teni::services {
class MediaLibrary final {
 public:
  explicit MediaLibrary(interfaces::IStorage& storage) : storage_(storage) {}
  bool scan(const std::string& directory);
  [[nodiscard]] bool empty() const { return tracks_.empty(); }
  [[nodiscard]] std::size_t size() const { return tracks_.size(); }
  [[nodiscard]] const std::string& at(std::size_t index) const { return tracks_.at(index); }
  [[nodiscard]] std::size_t find(const std::string& path) const;

 private:
  interfaces::IStorage& storage_;
  std::vector<std::string> tracks_;
};
}  // namespace teni::services
