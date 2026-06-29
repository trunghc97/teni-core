#include "modules/story/StoryPlayer.hpp"

namespace teni::modules::story {
StorySkill::StorySkill(services::MediaLibrary& library, interfaces::IAudioEngine& audio,
                       interfaces::IConfigStore& store, config::AppConfig& config)
    : library_(library), audio_(audio), store_(store), config_(config) {}
bool StorySkill::initialize() {
  library_.scan("/stories");
  if (library_.empty()) return true;
  index_ = library_.find(config_.currentStory);
  return true;
}
void StorySkill::shutdown() { saveBookmark(); }
bool StorySkill::play() {
  if (library_.empty()) return false;
  const bool resume = config_.currentStory == library_.at(index_);
  return playCurrent(resume ? config_.playbackPosition : 0);
}
void StorySkill::togglePause() {
  if (audio_.isPaused())
    audio_.resume();
  else if (audio_.isPlaying())
    audio_.pause();
  else
    play();
}
void StorySkill::pause() {
  if (audio_.isPlaying() && !audio_.isPaused()) audio_.pause();
}
void StorySkill::resume() {
  if (audio_.isPaused()) audio_.resume();
}
void StorySkill::stop() {
  saveBookmark();
  audio_.stop();
}
bool StorySkill::next() {
  if (library_.empty()) return false;
  index_ = (index_ + 1) % library_.size();
  config_.playbackPosition = 0;
  return playCurrent();
}
bool StorySkill::previous() {
  if (library_.empty()) return false;
  index_ = index_ == 0 ? library_.size() - 1 : index_ - 1;
  config_.playbackPosition = 0;
  return playCurrent();
}
bool StorySkill::random() {
  if (library_.empty()) return false;
  randomState_ = randomState_ * 1664525U + 1013904223U;
  index_ = randomState_ % library_.size();
  config_.playbackPosition = 0;
  return playCurrent();
}
void StorySkill::toggleRepeat() {
  config_.repeatStory = !config_.repeatStory;
  store_.save(config_);
}
void StorySkill::finished() {
  config_.playbackPosition = 0;
  if (config_.repeatStory)
    playCurrent();
  else if (config_.storyRandom)
    random();
  else
    next();
}
void StorySkill::saveBookmark() {
  if (!library_.empty()) config_.currentStory = library_.at(index_);
  if (audio_.isPlaying() || audio_.isPaused()) config_.playbackPosition = audio_.position();
  store_.save(config_);
}
bool StorySkill::playCurrent(std::uint32_t offset) {
  config_.currentStory = library_.at(index_);
  config_.playbackPosition = offset;
  store_.save(config_);
  return audio_.play(config_.currentStory, offset);
}
}  // namespace teni::modules::story
