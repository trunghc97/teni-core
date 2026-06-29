#include "services/AudioEngine.hpp"

#include <algorithm>

#include "core/Logger.hpp"
#ifdef ARDUINO
#include <AudioFileSourcePROGMEM.h>
#include <AudioGenerator.h>
#include <AudioGeneratorMP3.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>
#endif

namespace teni::services {

AudioEngine::AudioEngine(interfaces::IEventBus& bus, interfaces::IClock& clock,
                         interfaces::IAssetManager& assets, int bclk, int lrclk, int dout)
    : bus_(bus), clock_(clock), assets_(assets), bclk_(bclk), lrclk_(lrclk), dout_(dout) {}
AudioEngine::~AudioEngine() { stop(); }

bool AudioEngine::begin() {
#ifdef ARDUINO
  output_ = std::make_unique<AudioOutputI2S>(0, AudioOutputI2S::EXTERNAL_I2S);
  if (!output_->SetPinout(bclk_, lrclk_, dout_)) return false;
  setVolume(volume_);
  return true;
#else
  return false;
#endif
}

bool AudioEngine::play(const std::string& path, std::uint32_t byteOffset) {
  stop();
#ifdef ARDUINO
  interfaces::AssetView asset;
  if (!assets_.findAsset(path, asset) || asset.data == nullptr || asset.size == 0) return false;
  source_ = std::make_unique<AudioFileSourcePROGMEM>(asset.data, asset.size);
  if (byteOffset > 0) source_->seek(static_cast<int32_t>(byteOffset), SEEK_SET);
  const auto dot = path.find_last_of('.');
  std::string extension = dot == std::string::npos ? "" : path.substr(dot);
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
  if (extension == ".mp3")
    decoder_ = std::make_unique<AudioGeneratorMP3>();
  else if (extension == ".wav")
    decoder_ = std::make_unique<AudioGeneratorWAV>();
  else {
    source_.reset();
    return false;
  }
  if (!decoder_->begin(source_.get(), output_.get())) {
    decoder_.reset();
    source_.reset();
    return false;
  }
  playing_ = true;
  paused_ = false;
  bus_.publish({events::Type::MediaStarted, clock_.milliseconds()});
  return true;
#else
  (void)path;
  (void)byteOffset;
  return false;
#endif
}

void AudioEngine::pause() {
  if (!playing_ || paused_) return;
  paused_ = true;
  bus_.publish({events::Type::MediaPaused, clock_.milliseconds()});
}
void AudioEngine::resume() {
  if (!playing_ || !paused_) return;
  paused_ = false;
  bus_.publish({events::Type::MediaResumed, clock_.milliseconds()});
}
void AudioEngine::stop() {
#ifdef ARDUINO
  if (decoder_) decoder_->stop();
  decoder_.reset();
  source_.reset();
#endif
  if (playing_) bus_.publish({events::Type::MediaStopped, clock_.milliseconds()});
  playing_ = false;
  paused_ = false;
}
void AudioEngine::setVolume(std::uint8_t percent) {
  volume_ = std::min<std::uint8_t>(percent, 100);
#ifdef ARDUINO
  if (output_) output_->SetGain(static_cast<float>(volume_) / 100.0F);
#endif
}
std::uint32_t AudioEngine::position() const {
#ifdef ARDUINO
  return source_ ? static_cast<std::uint32_t>(source_->getPos()) : 0;
#else
  return 0;
#endif
}
void AudioEngine::service() {
#ifdef ARDUINO
  if (!playing_ || paused_ || !decoder_) return;
  if (!decoder_->isRunning() || !decoder_->loop()) {
    decoder_->stop();
    decoder_.reset();
    source_.reset();
    playing_ = false;
    bus_.publish({events::Type::MediaFinished, clock_.milliseconds()});
  }
#endif
}

}  // namespace teni::services
