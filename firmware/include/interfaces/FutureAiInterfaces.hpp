#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace teni::interfaces {

struct RecognizedIntent final {
  std::string name;
  std::string payload;
  float confidence{0.0F};
};

class IWakeWord {
 public:
  virtual ~IWakeWord() = default;
  virtual bool detect(const std::int16_t*, std::size_t) = 0;
};
class ISpeechRecognizer {
 public:
  virtual ~ISpeechRecognizer() = default;
  virtual std::string recognize(const std::int16_t*, std::size_t) = 0;
};
class IIntentRecognizer {
 public:
  virtual ~IIntentRecognizer() = default;
  virtual RecognizedIntent recognize(const std::string&) = 0;
};
class ILLM {
 public:
  virtual ~ILLM() = default;
  virtual std::string complete(const std::string&) = 0;
};
class ITTS {
 public:
  virtual ~ITTS() = default;
  virtual bool synthesize(const std::string&, const std::string&) = 0;
};
class IMemory {
 public:
  virtual ~IMemory() = default;
  virtual void remember(const std::string&, const std::string&) = 0;
  virtual std::string recall(const std::string&) = 0;
};
class IKnowledgeBase {
 public:
  virtual ~IKnowledgeBase() = default;
  virtual std::vector<std::string> search(const std::string&, std::size_t) = 0;
};

}  // namespace teni::interfaces
