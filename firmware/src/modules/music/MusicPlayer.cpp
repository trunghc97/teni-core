#include "modules/music/MusicPlayer.hpp"

#include <algorithm>

namespace teni::modules::music {
MusicSkill::MusicSkill(services::MediaLibrary& library, interfaces::IAudioEngine& audio,
                       interfaces::IConfigStore& store, config::AppConfig& config)
    : library_(library), audio_(audio), store_(store), config_(config) {}
bool MusicSkill::initialize() {
  library_.scan("/music");
  return true;
}
bool MusicSkill::play() { return library_.empty() ? false : playCurrent(); }
void MusicSkill::togglePause() {
  if (audio_.isPaused())
    audio_.resume();
  else if (audio_.isPlaying())
    audio_.pause();
  else
    play();
}
void MusicSkill::pause() {
  if (audio_.isPlaying() && !audio_.isPaused()) audio_.pause();
}
void MusicSkill::resume() {
  if (audio_.isPaused()) audio_.resume();
}
bool MusicSkill::next() {
  if (library_.empty()) return false;
  if (config_.musicShuffle) {
    randomState_ = randomState_ * 1664525U + 1013904223U;
    index_ = randomState_ % library_.size();
  } else
    index_ = (index_ + 1) % library_.size();
  return playCurrent();
}
bool MusicSkill::previous() {
  if (library_.empty()) return false;
  index_ = index_ == 0 ? library_.size() - 1 : index_ - 1;
  return playCurrent();
}
void MusicSkill::toggleShuffle() {
  config_.musicShuffle = !config_.musicShuffle;
  store_.save(config_);
}
void MusicSkill::changeVolume(int delta) {
  const int volume = std::clamp(static_cast<int>(config_.volume) + delta, 0, 100);
  config_.volume = static_cast<std::uint8_t>(volume);
  audio_.setVolume(config_.volume);
  store_.save(config_);
}
void MusicSkill::finished() { next(); }
bool MusicSkill::playCurrent() { return audio_.play(library_.at(index_)); }
}  // namespace teni::modules::music
