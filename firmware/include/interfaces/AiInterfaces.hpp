#pragma once

#include <cstdint>
#include <string>

namespace teni::interfaces {

enum class Intent : std::uint8_t {
  PlayStory,
  PlayMusic,
  Pause,
  Resume,
  Stop,
  VolumeUp,
  VolumeDown,
  Greeting,
  Animal,
  Color,
  Alphabet,
  Number,
  Math,
  Bedtime,
  Unknown
};

struct AIResult final {
  Intent intent{Intent::Unknown};
  float confidence{0.0F};
  std::string response;
  std::string responseAudioAsset;
};

class IIntentMatcher {
 public:
  virtual ~IIntentMatcher() = default;
  [[nodiscard]] virtual Intent match(const std::string& question, float& confidence) const = 0;
};

/** Replaceable inference boundary; application and skills depend only on this contract. */
class IAIEngine {
 public:
  virtual ~IAIEngine() = default;
  [[nodiscard]] virtual AIResult process(const std::string& question) const = 0;
};

// Reserved future implementation. No LLM code is included in MVP1.1.
class LLMEngine;
class LocalLLMEngine;

}  // namespace teni::interfaces
