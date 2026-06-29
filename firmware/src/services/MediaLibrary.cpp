#include "services/MediaLibrary.hpp"

#include <algorithm>

namespace teni::services {
bool MediaLibrary::scan(const std::string& directory) {
  tracks_ = storage_.list(directory, ".mp3");
  const auto wav = storage_.list(directory, ".wav");
  tracks_.insert(tracks_.end(), wav.begin(), wav.end());
  std::sort(tracks_.begin(), tracks_.end());
  return !tracks_.empty();
}
std::size_t MediaLibrary::find(const std::string& path) const {
  const auto iterator = std::find(tracks_.begin(), tracks_.end(), path);
  return iterator == tracks_.end() ? 0 : static_cast<std::size_t>(iterator - tracks_.begin());
}
}  // namespace teni::services
