#include "app/TeniApplication.hpp"

#ifdef ARDUINO
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif

#include "config/BoardConfig.hpp"
#include "core/Logger.hpp"

namespace teni::app {
using config::BoardPins;

TeniApplication::TeniApplication()
    : bus_(48),
      systemTimer_("system", 1000,
                   [this] { bus_.publish({events::Type::Tick, clock_.milliseconds()}); }),
      battery_(BoardPins::kBatteryAdc, BoardPins::kChargeDetect,
               config::HardwareLimits::kBatteryDividerRatio),
      led_(BoardPins::kRgbLed),
      power_(BoardPins::kButtonPlay, BoardPins::kPowerHold),
      audio_(bus_, clock_, assets_, BoardPins::kI2sBclk, BoardPins::kI2sLrclk, BoardPins::kI2sDout),
      storyLibrary_(assets_),
      musicLibrary_(assets_),
      stateMachine_(bus_),
      system_(bus_, clock_, battery_, led_, power_, config_),
      buttons_(bus_, [](int pin) {
#ifdef ARDUINO
        return digitalRead(pin) != LOW;
#else
        (void)pin; return true;
#endif
      }) {
}

bool TeniApplication::initialize() {
  LOG_I("App", "Teni Core MVP1.1 booting");
  configStore_.load(config_);
  led_.begin();
  battery_.begin();
  power_.begin();
  if (!assets_.begin()) return false;
  LOG_I("Assets", "%u embedded bytes", static_cast<unsigned>(assets_.embeddedBytes()));
  ai_ = services::createAIEngine(assets_);
  if (!audio_.begin()) {
    LOG_E("App", "I2S audio initialization failed");
    return false;
  }
  audio_.setVolume(config_.volume);
  story_ = &skills_.add<modules::story::StorySkill>(storyLibrary_, audio_, configStore_, config_);
  music_ = &skills_.add<modules::music::MusicSkill>(musicLibrary_, audio_, configStore_, config_);
  playback_ = std::make_unique<services::PlaybackCoordinator>(bus_, *story_, *music_);
  interaction_ = std::make_unique<services::AIInteractionService>(*ai_, *playback_, audio_);
  knowledge_ = &skills_.add<modules::knowledge::KnowledgeSkill>(*interaction_);
  if (!skills_.initializeAll()) return false;
  configureButtons();
  wireEvents();
  bus_.publish({events::Type::SystemReady, clock_.milliseconds()});
  if (!systemTimer_.start()) return false;
  runtimeSignals_.set(0x01U);
  if (!config_.currentStory.empty() && config_.playbackPosition > 0) story_->play();
  return true;
}

void TeniApplication::configureButtons() {
#ifdef ARDUINO
  for (int pin : {BoardPins::kButtonPlay, BoardPins::kButtonNext, BoardPins::kButtonPrevious,
                  BoardPins::kButtonMode})
    pinMode(pin, INPUT_PULLUP);
#endif
  buttons_.add({events::ButtonId::Play, BoardPins::kButtonPlay});
  buttons_.add({events::ButtonId::Next, BoardPins::kButtonNext});
  buttons_.add({events::ButtonId::Previous, BoardPins::kButtonPrevious});
  buttons_.add({events::ButtonId::Mode, BoardPins::kButtonMode});
}

void TeniApplication::wireEvents() {
  bus_.subscribe(events::Type::ButtonGesture, [this](const auto& event) {
    system_.handle(event);
    playback_->handleButton(event);
  });
  bus_.subscribe(events::Type::Tick, [this](const auto&) { system_.tick(); });
  bus_.subscribe(events::Type::MediaFinished, [this](const auto& event) {
    stateMachine_.handle(event);
    playback_->handleFinished(event);
  });
  for (auto type :
       {events::Type::SystemReady, events::Type::MediaStarted, events::Type::MediaPaused,
        events::Type::MediaResumed, events::Type::MediaStopped, events::Type::LowBattery,
        events::Type::CriticalBattery, events::Type::ChargeStarted, events::Type::ChargeStopped,
        events::Type::SleepRequested, events::Type::ShutdownRequested}) {
    bus_.subscribe(type, [this](const auto& event) { stateMachine_.handle(event); });
  }
  for (auto type : {events::Type::CriticalBattery, events::Type::SleepRequested,
                    events::Type::ShutdownRequested}) {
    bus_.subscribe(type, [this](const auto&) { saveBookmark(); });
  }
  bus_.subscribe(events::Type::StateChanged, [this](const auto& event) {
    LOG_I("State", "%s -> %s",
          core::DeviceStateMachine::name(static_cast<core::DeviceState>(event.value)),
          core::DeviceStateMachine::name(static_cast<core::DeviceState>(event.value2)));
    system_.handle(event);
  });
}

void TeniApplication::saveBookmark() { story_->saveBookmark(); }

[[noreturn]] void TeniApplication::run() {
#ifdef ARDUINO
  if (!runtimeSignals_.has(0x01U)) std::terminate();
  TickType_t wake = xTaskGetTickCount();
  while (true) {
    const auto now = clock_.milliseconds();
    buttons_.poll(now);
    audio_.service();
    bus_.dispatchPending(24);
    if (audio_.isPlaying() && now - lastBookmark_ >= config::HardwareLimits::kBookmarkPeriodMs) {
      lastBookmark_ = now;
      saveBookmark();
    }
    vTaskDelayUntil(&wake, pdMS_TO_TICKS(config::HardwareLimits::kButtonPollMs));
  }
#else
  std::terminate();
#endif
}
}  // namespace teni::app
