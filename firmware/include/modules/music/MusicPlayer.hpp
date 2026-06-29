#pragma once

#include <cstddef>

#include "config/AppConfig.hpp"
#include "interfaces/CoreInterfaces.hpp"
#include "services/MediaLibrary.hpp"

namespace teni::modules::music {
class MusicSkill final : public interfaces::ISkill {
 public:
  MusicSkill(services::MediaLibrary& library, interfaces::IAudioEngine& audio,
             interfaces::IConfigStore& store, config::AppConfig& config);
  [[nodiscard]] const char* name() const override { return "music"; }
  bool initialize() override;
  void shutdown() override { audio_.stop(); }
  bool play();
  void togglePause();
  void pause();
  void resume();
  void stop() { audio_.stop(); }
  bool next();
  bool previous();
  void toggleShuffle();
  void changeVolume(int delta);
  void finished();

 private:
  bool playCurrent();
  services::MediaLibrary& library_;
  interfaces::IAudioEngine& audio_;
  interfaces::IConfigStore& store_;
  config::AppConfig& config_;
  std::size_t index_{0};
  std::uint32_t randomState_{0x4D555349U};
};
using MusicPlayer = MusicSkill;
}  // namespace teni::modules::music
