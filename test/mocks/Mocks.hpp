#pragma once

#include <map>
#include <string>
#include <vector>

#include "interfaces/CoreInterfaces.hpp"

namespace teni::test {
class MockClock final : public interfaces::IClock {
 public:
  std::uint32_t now{0};
  [[nodiscard]] std::uint32_t milliseconds() const override { return now; }
};
class MockStorage final : public interfaces::IStorage {
 public:
  std::map<std::string, std::vector<std::string>> files;
  bool begin() override { return true; }
  bool exists(const std::string& path) const override { return text.count(path) != 0; }
  std::vector<std::string> list(const std::string& directory,
                                const std::string& extension) const override {
    auto iterator = files.find(directory + extension);
    return iterator == files.end() ? std::vector<std::string>{} : iterator->second;
  }
  bool readText(const std::string& path, std::string& output) const override {
    auto it = text.find(path);
    if (it == text.end()) return false;
    output = it->second;
    return true;
  }
  bool writeTextAtomic(const std::string& path, const std::string& value) override {
    text[path] = value;
    return true;
  }
  mutable std::map<std::string, std::string> text;
};
class MockConfigStore final : public interfaces::IConfigStore {
 public:
  bool load(config::AppConfig& value) override {
    value = saved;
    return true;
  }
  bool save(const config::AppConfig& value) override {
    saved = value;
    ++saveCount;
    return true;
  }
  config::AppConfig saved{};
  int saveCount{0};
};
class MockAudio final : public interfaces::IAudioEngine {
 public:
  bool play(const std::string& path, std::uint32_t offset) override {
    lastPath = path;
    lastOffset = offset;
    playing = true;
    paused = false;
    return playResult;
  }
  void pause() override { paused = true; }
  void resume() override { paused = false; }
  void stop() override {
    playing = false;
    paused = false;
  }
  void setVolume(std::uint8_t value) override { volume = value; }
  bool isPlaying() const override { return playing; }
  bool isPaused() const override { return paused; }
  std::uint32_t position() const override { return currentPosition; }
  std::string lastPath;
  std::uint32_t lastOffset{0};
  std::uint32_t currentPosition{0};
  std::uint8_t volume{0};
  bool playing{false};
  bool paused{false};
  bool playResult{true};
};
}  // namespace teni::test
