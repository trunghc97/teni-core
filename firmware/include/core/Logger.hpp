#pragma once

#include <cstdint>

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdio>
#endif

#define TENI_LOG_ERROR 0
#define TENI_LOG_WARN 1
#define TENI_LOG_INFO 2
#define TENI_LOG_DEBUG 3
#ifndef TENI_LOG_LEVEL
#define TENI_LOG_LEVEL TENI_LOG_INFO
#endif

namespace teni::core {
class Logger final {
 public:
  template <typename... Args>
  static void write(const char* level, const char* tag, const char* format, Args... args) {
#ifdef ARDUINO
    Serial.printf("[%s] [%s] ", level, tag);
    Serial.printf(format, args...);
    Serial.println();
#else
    std::printf("[%s] [%s] ", level, tag);
    std::printf(format, args...);
    std::printf("\n");
#endif
  }
};
}  // namespace teni::core

#if TENI_LOG_LEVEL >= TENI_LOG_ERROR
#define LOG_E(tag, fmt, ...) ::teni::core::Logger::write("ERROR", tag, fmt, ##__VA_ARGS__)
#else
#define LOG_E(tag, fmt, ...)
#endif
#if TENI_LOG_LEVEL >= TENI_LOG_WARN
#define LOG_W(tag, fmt, ...) ::teni::core::Logger::write("WARN", tag, fmt, ##__VA_ARGS__)
#else
#define LOG_W(tag, fmt, ...)
#endif
#if TENI_LOG_LEVEL >= TENI_LOG_INFO
#define LOG_I(tag, fmt, ...) ::teni::core::Logger::write("INFO", tag, fmt, ##__VA_ARGS__)
#else
#define LOG_I(tag, fmt, ...)
#endif
#if TENI_LOG_LEVEL >= TENI_LOG_DEBUG
#define LOG_D(tag, fmt, ...) ::teni::core::Logger::write("DEBUG", tag, fmt, ##__VA_ARGS__)
#else
#define LOG_D(tag, fmt, ...)
#endif
