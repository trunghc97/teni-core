#include "services/SimpleAIEngine.hpp"

#include <cctype>

namespace teni::services {

SimpleAIEngine::SimpleAIEngine(interfaces::IAssetManager& assets)
    : SimpleAIEngine(assets, std::make_unique<IntentMatcher>()) {}

SimpleAIEngine::SimpleAIEngine(interfaces::IAssetManager& assets,
                               std::unique_ptr<interfaces::IIntentMatcher> matcher)
    : assets_(assets), matcher_(std::move(matcher)) {}

const char* SimpleAIEngine::topicFor(interfaces::Intent intent) {
  using interfaces::Intent;
  switch (intent) {
    case Intent::Greeting:
      return "greetings";
    case Intent::Animal:
      return "animals";
    case Intent::Color:
      return "colors";
    case Intent::Alphabet:
      return "alphabet";
    case Intent::Number:
      return "numbers";
    case Intent::Math:
      return "math";
    case Intent::Bedtime:
      return "bedtime";
    default:
      return nullptr;
  }
}

const char* SimpleAIEngine::templateFor(interfaces::Intent intent) {
  using interfaces::Intent;
  switch (intent) {
    case Intent::Unknown:
      return "unknown";
    case Intent::Greeting:
      return "greeting";
    case Intent::Animal:
    case Intent::Color:
    case Intent::Alphabet:
    case Intent::Number:
    case Intent::Math:
    case Intent::Bedtime:
      return "not_found";
    default:
      return "acknowledge";
  }
}

std::string SimpleAIEngine::lookupKnowledge(interfaces::Intent intent,
                                            const std::string& normalized) const {
  const char* topic = topicFor(intent);
  if (topic == nullptr) return {};
  if (intent == interfaces::Intent::Math) {
    std::string mathKey;
    char operation = '\0';
    for (const unsigned char character : normalized) {
      if (std::isdigit(character)) mathKey.push_back(static_cast<char>(character));
      if (operation == '\0' && (character == '+' || character == '-')) operation = character;
    }
    if (operation == '\0') {
      if (normalized.find("cộng") != std::string::npos ||
          normalized.find("plus") != std::string::npos)
        operation = '+';
      else if (normalized.find("trừ") != std::string::npos ||
               normalized.find("minus") != std::string::npos)
        operation = '-';
    }
    if (mathKey.size() >= 2 && operation != '\0') {
      const std::string key{mathKey.front(), operation, mathKey.back()};
      const auto value = assets_.knowledge(topic, key);
      if (!value.empty()) return value;
    }
  }
  for (const auto& key : assets_.knowledgeKeys(topic)) {
    if (normalized.find(key) != std::string::npos) return assets_.knowledge(topic, key);
  }
  if (intent == interfaces::Intent::Greeting) return assets_.knowledge(topic, "hello");
  if (intent == interfaces::Intent::Bedtime) return assets_.knowledge(topic, "goodnight");
  return {};
}

interfaces::AIResult SimpleAIEngine::process(const std::string& question) const {
  interfaces::AIResult result;
  result.intent = matcher_->match(question, result.confidence);
  result.response = lookupKnowledge(result.intent, IntentMatcher::normalize(question));
  if (result.response.empty())
    result.response = assets_.responseTemplate(templateFor(result.intent));
  const std::string audioPath = "/responses/" + std::string(templateFor(result.intent)) + ".mp3";
  if (assets_.exists(audioPath)) result.responseAudioAsset = audioPath;
  return result;
}

}  // namespace teni::services
