#pragma once

#include <memory>

#include "config/AppConfig.hpp"
#include "core/DeviceStateMachine.hpp"
#include "core/EventBus.hpp"
#include "core/SkillRegistry.hpp"
#include "drivers/ButtonEngine.hpp"
#include "drivers/Esp32Drivers.hpp"
#include "hal/FreeRtosPrimitives.hpp"
#include "interfaces/AiInterfaces.hpp"
#include "modules/knowledge/KnowledgeSkill.hpp"
#include "modules/music/MusicSkill.hpp"
#include "modules/story/StorySkill.hpp"
#include "services/AIEngineFactory.hpp"
#include "services/AIInteractionService.hpp"
#include "services/AssetManager.hpp"
#include "services/AudioEngine.hpp"
#include "services/MediaLibrary.hpp"
#include "services/PlaybackCoordinator.hpp"
#include "services/SystemService.hpp"

namespace teni::app {
class TeniApplication final {
 public:
  TeniApplication();
  bool initialize();
  [[noreturn]] void run();

 private:
  void wireEvents();
  void configureButtons();
  void saveBookmark();

  config::AppConfig config_;
  core::EventBus bus_;
  drivers::ArduinoClock clock_;
  hal::EventGroup runtimeSignals_;
  hal::PeriodicTimer systemTimer_;
  drivers::NvsConfigStore configStore_;
  services::AssetManager assets_;
  std::unique_ptr<interfaces::IAIEngine> ai_;
  drivers::Esp32Battery battery_;
  drivers::Esp32RgbLed led_;
  drivers::Esp32Power power_;
  services::AudioEngine audio_;
  services::MediaLibrary storyLibrary_;
  services::MediaLibrary musicLibrary_;
  core::SkillRegistry skills_;
  modules::story::StorySkill* story_{nullptr};
  modules::music::MusicSkill* music_{nullptr};
  modules::knowledge::KnowledgeSkill* knowledge_{nullptr};
  std::unique_ptr<services::PlaybackCoordinator> playback_;
  std::unique_ptr<services::AIInteractionService> interaction_;
  core::DeviceStateMachine stateMachine_;
  services::SystemService system_;
  drivers::ButtonEngine buttons_;
  std::uint32_t lastBookmark_{0};
};
}  // namespace teni::app
