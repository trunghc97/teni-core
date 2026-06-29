#include "services/PlaybackCoordinator.hpp"

namespace teni::services {
PlaybackCoordinator::PlaybackCoordinator(interfaces::IEventBus& bus,
                                         modules::story::StorySkill& story,
                                         modules::music::MusicSkill& music)
    : bus_(bus), story_(story), music_(music) {}
void PlaybackCoordinator::handleButton(const events::Event& event) {
  const auto button = static_cast<events::ButtonId>(event.value);
  const auto gesture = static_cast<events::Gesture>((event.value2 >> 8) & 0xFF);
  if (button == events::ButtonId::Mode && gesture == events::Gesture::ShortPress) {
    story_.stop();
    music_.stop();
    mode_ = mode_ == Mode::Story ? Mode::Music : Mode::Story;
    return;
  }
  if (button == events::ButtonId::Play) {
    if (gesture == events::Gesture::LongPress) {
      if (mode_ == Mode::Story)
        story_.stop();
      else
        music_.stop();
    } else if (mode_ == Mode::Story)
      story_.togglePause();
    else
      music_.togglePause();
  } else if (button == events::ButtonId::Next) {
    if (gesture == events::Gesture::LongPress && mode_ == Mode::Music)
      music_.changeVolume(5);
    else if (mode_ == Mode::Story)
      story_.next();
    else
      music_.next();
  } else if (button == events::ButtonId::Previous) {
    if (gesture == events::Gesture::LongPress && mode_ == Mode::Music)
      music_.changeVolume(-5);
    else if (mode_ == Mode::Story)
      story_.previous();
    else
      music_.previous();
  } else if (button == events::ButtonId::Mode && gesture == events::Gesture::DoubleClick) {
    if (mode_ == Mode::Story)
      story_.random();
    else
      music_.toggleShuffle();
  } else if (button == events::ButtonId::Mode && gesture == events::Gesture::LongPress &&
             mode_ == Mode::Story)
    story_.toggleRepeat();
}
void PlaybackCoordinator::handleFinished(const events::Event&) {
  if (mode_ == Mode::Story)
    story_.finished();
  else
    music_.finished();
}
void PlaybackCoordinator::handleIntent(interfaces::Intent intent) {
  using interfaces::Intent;
  switch (intent) {
    case Intent::PlayStory:
      music_.stop();
      mode_ = Mode::Story;
      story_.play();
      break;
    case Intent::PlayMusic:
      story_.stop();
      mode_ = Mode::Music;
      music_.play();
      break;
    case Intent::Pause:
      if (mode_ == Mode::Story)
        story_.pause();
      else
        music_.pause();
      break;
    case Intent::Resume:
      if (mode_ == Mode::Story)
        story_.resume();
      else
        music_.resume();
      break;
    case Intent::Stop:
      if (mode_ == Mode::Story)
        story_.stop();
      else
        music_.stop();
      break;
    case Intent::VolumeUp:
      music_.changeVolume(5);
      break;
    case Intent::VolumeDown:
      music_.changeVolume(-5);
      break;
    default:
      break;
  }
}
}  // namespace teni::services
