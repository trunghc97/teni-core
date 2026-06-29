#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "interfaces/CoreInterfaces.hpp"

#ifdef ARDUINO
class AudioFileSourcePROGMEM;
class AudioGenerator;
class AudioOutputI2S;
#endif

namespace teni::services {

class AudioEngine final : public interfaces::IAudioEngine {
 public:
  AudioEngine(interfaces::IEventBus& bus, interfaces::IClock& clock,
              interfaces::IAssetManager& assets, int bclk, int lrclk, int dout);
  ~AudioEngine() override;
  bool begin();
  bool play(const std::string& path, std::uint32_t byteOffset = 0) override;
  void pause() override;
  void resume() override;
  void stop() override;
  void setVolume(std::uint8_t percent) override;
  [[nodiscard]] bool isPlaying() const override { return playing_; }
  [[nodiscard]] bool isPaused() const override { return paused_; }
  [[nodiscard]] std::uint32_t position() const override;
  void service();

 private:
  interfaces::IEventBus& bus_;
  interfaces::IClock& clock_;
  interfaces::IAssetManager& assets_;
  int bclk_, lrclk_, dout_;
  bool playing_{false};
  bool paused_{false};
  std::uint8_t volume_{65};
#ifdef ARDUINO
  std::unique_ptr<AudioOutputI2S> output_;
  std::unique_ptr<AudioFileSourcePROGMEM> source_;
  std::unique_ptr<AudioGenerator> decoder_;
#endif
};

}  // namespace teni::services
