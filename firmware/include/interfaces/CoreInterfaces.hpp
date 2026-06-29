#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "config/AppConfig.hpp"
#include "events/Event.hpp"

namespace teni::interfaces {

class IEventBus {
 public:
  using Handler = std::function<void(const events::Event&)>;
  virtual ~IEventBus() = default;
  virtual bool publish(const events::Event& event) = 0;
  virtual void subscribe(events::Type type, Handler handler) = 0;
  virtual void dispatchPending(std::size_t maximum = 16) = 0;
};

class IClock {
 public:
  virtual ~IClock() = default;
  [[nodiscard]] virtual std::uint32_t milliseconds() const = 0;
};

class IStorage {
 public:
  virtual ~IStorage() = default;
  virtual bool begin() = 0;
  [[nodiscard]] virtual bool exists(const std::string& path) const = 0;
  [[nodiscard]] virtual std::vector<std::string> list(const std::string& directory,
                                                      const std::string& extension) const = 0;
  virtual bool readText(const std::string& path, std::string& output) const = 0;
  virtual bool writeTextAtomic(const std::string& path, const std::string& content) = 0;
};

struct AssetView final {
  const std::uint8_t* data{nullptr};
  std::size_t size{0};
  std::string mime;
};

/** Read-only registry for build-time assets and knowledge stored in MCU flash. */
class IAssetManager : public IStorage {
 public:
  ~IAssetManager() override = default;
  virtual bool registerAsset(const std::string& id, const std::string& path,
                             const std::string& mime, const std::uint8_t* data,
                             std::size_t size) = 0;
  [[nodiscard]] virtual bool findAsset(const std::string& idOrPath, AssetView& view) const = 0;
  [[nodiscard]] virtual std::string knowledge(const std::string& topic,
                                              const std::string& key) const = 0;
  [[nodiscard]] virtual std::vector<std::string> knowledgeKeys(const std::string& topic) const = 0;
  [[nodiscard]] virtual std::string responseTemplate(const std::string& key) const = 0;
  [[nodiscard]] virtual std::size_t embeddedBytes() const = 0;
};

class IConfigStore {
 public:
  virtual ~IConfigStore() = default;
  virtual bool load(config::AppConfig& config) = 0;
  virtual bool save(const config::AppConfig& config) = 0;
};

class IAudioEngine {
 public:
  virtual ~IAudioEngine() = default;
  virtual bool play(const std::string& path, std::uint32_t byteOffset = 0) = 0;
  virtual void pause() = 0;
  virtual void resume() = 0;
  virtual void stop() = 0;
  virtual void setVolume(std::uint8_t percent) = 0;
  [[nodiscard]] virtual bool isPlaying() const = 0;
  [[nodiscard]] virtual bool isPaused() const = 0;
  [[nodiscard]] virtual std::uint32_t position() const = 0;
};

class ILed {
 public:
  virtual ~ILed() = default;
  virtual void setRgb(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
                      std::uint8_t brightness) = 0;
  virtual void off() = 0;
};

class IBattery {
 public:
  virtual ~IBattery() = default;
  [[nodiscard]] virtual float voltage() const = 0;
  [[nodiscard]] virtual std::uint8_t percent() const = 0;
  [[nodiscard]] virtual bool charging() const = 0;
};

class IPower {
 public:
  virtual ~IPower() = default;
  virtual void deepSleep() = 0;
  virtual void powerOff() = 0;
};

class IPlugin {
 public:
  virtual ~IPlugin() = default;
  [[nodiscard]] virtual const char* id() const = 0;
  [[nodiscard]] virtual const char* version() const = 0;
  virtual bool initialize() = 0;
  virtual void shutdown() = 0;
};

class ISkill : public IPlugin {
 public:
  ~ISkill() override = default;
  [[nodiscard]] virtual const char* name() const = 0;
  [[nodiscard]] const char* id() const final { return name(); }
  [[nodiscard]] const char* version() const override { return "1.1.0"; }
};

}  // namespace teni::interfaces
