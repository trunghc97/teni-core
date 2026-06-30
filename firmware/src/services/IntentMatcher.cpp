#include "services/IntentMatcher.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <limits>
#include <sstream>

namespace teni::services {
namespace {
using interfaces::Intent;
const std::vector<IntentMatcher::Rule> kRules{
    {Intent::Stop, 100, {"dừng lại", "stop playback"}, {"stop", "dừng"}},
    {Intent::Pause, 95, {"tạm dừng", "pause playback"}, {"pause"}},
    {Intent::Resume, 94, {"tiếp tục", "resume playback"}, {"resume"}},
    {Intent::VolumeUp, 90, {"tăng âm lượng", "volume up"}, {"louder"}},
    {Intent::VolumeDown, 90, {"giảm âm lượng", "volume down"}, {"quieter"}},
    {Intent::PlayStory, 85, {"kể chuyện", "play story"}, {"story", "truyện"}},
    {Intent::PlayMusic, 84, {"phát nhạc", "play music"}, {"music", "nhạc"}},
    {Intent::Math, 70, {"cộng", "trừ", "plus", "minus"}, {"math", "+", "-"}},
    {Intent::Animal, 60, {"con vật", "động vật"}, {"animal", "mèo", "chó", "voi"}},
    {Intent::Color, 60, {"màu gì", "màu sắc"}, {"color", "đỏ", "vàng", "xanh"}},
    {Intent::Alphabet, 60, {"chữ cái", "bảng chữ cái"}, {"alphabet"}},
    {Intent::Number, 60, {"con số", "số mấy"}, {"number"}},
    {Intent::Conversation,
     58,
     {"khỏe không", "đang làm gì", "thích gì", "bao nhiêu tuổi", "cảm ơn", "tạm biệt",
      "mình buồn", "mình vui", "mình sợ", "làm được gì", "có thể làm gì", "làm bạn",
      "bạn ở đâu", "teni ở đâu", "phải robot"},
     {}},
    {Intent::Bedtime, 55, {"đi ngủ", "ngủ ngon"}, {"bedtime", "sleep"}},
    {Intent::Greeting,
     50,
     {"xin chào", "chào teni", "bạn tên gì", "tên của bạn", "teni tên gì", "bạn là ai",
      "cậu là ai", "ai vậy"},
     {"hello", "hi"}},
};
}  // namespace

std::string IntentMatcher::normalize(const std::string& text) {
  std::string result;
  result.reserve(text.size());
  bool previousSpace = true;
  for (const unsigned char character : text) {
    if (character < 128 && (std::isalnum(character) || character == '+' || character == '-')) {
      result.push_back(static_cast<char>(std::tolower(character)));
      previousSpace = false;
    } else if (character >= 128) {
      result.push_back(static_cast<char>(character));
      previousSpace = false;
    } else if (!previousSpace) {
      result.push_back(' ');
      previousSpace = true;
    }
  }
  if (!result.empty() && result.back() == ' ') result.pop_back();
  return result;
}

std::size_t IntentMatcher::editDistance(const std::string& left, const std::string& right) {
  std::vector<std::size_t> previous(right.size() + 1);
  std::vector<std::size_t> current(right.size() + 1);
  for (std::size_t column = 0; column <= right.size(); ++column) previous[column] = column;
  for (std::size_t row = 1; row <= left.size(); ++row) {
    current[0] = row;
    for (std::size_t column = 1; column <= right.size(); ++column) {
      const auto substitution = previous[column - 1] + (left[row - 1] == right[column - 1] ? 0 : 1);
      current[column] = std::min({previous[column] + 1, current[column - 1] + 1, substitution});
    }
    previous.swap(current);
  }
  return previous[right.size()];
}

interfaces::Intent IntentMatcher::match(const std::string& question, float& confidence) const {
  const std::string normalized = normalize(question);
  Intent best = Intent::Unknown;
  int bestScore = 0;
  std::vector<std::string> words;
  std::istringstream stream(normalized);
  for (std::string word; stream >> word;) words.push_back(std::move(word));

  for (const auto& rule : kRules) {
    int score = 0;
    for (const char* pattern : rule.patterns) {
      if (normalized.find(pattern) != std::string::npos)
        score = std::max(score, rule.priority + 20);
    }
    for (const char* keyword : rule.keywords) {
      const std::string keywordText(keyword);
      const bool symbolic = keywordText == "+" || keywordText == "-";
      const bool exactWord = std::find(words.begin(), words.end(), keywordText) != words.end();
      if ((symbolic && normalized.find(keywordText) != std::string::npos) || exactWord)
        score = std::max(score, rule.priority + 10);
      if (std::strlen(keyword) >= 4) {
        for (const auto& word : words) {
          if (editDistance(word, keyword) == 1) score = std::max(score, rule.priority);
        }
      }
    }
    if (score > bestScore) {
      bestScore = score;
      best = rule.intent;
    }
  }
  confidence = bestScore == 0 ? 0.0F : std::min(1.0F, static_cast<float>(bestScore) / 120.0F);
  return best;
}

}  // namespace teni::services
