#pragma once

#include <memory>
#include <string>

#include "logger.hpp"

namespace ebroschin::logging {

class Log {
public:
  static void Verbose(const std::string&);
  static void Debug(const std::string&);
  static void Info(const std::string&);
  static void Warning(const std::string&);
  static void Error(const std::string&);
  static void Critical(const std::string&);
  static void Print(LogLevel, const std::string&);

  static void SetLogLevel(LogLevel);

  template <typename TLogger, typename... TArguments>
  static void SetLogger(TArguments&&... arguments) {
    SetLogger(std::make_shared<TLogger>(std::forward<TArguments>(arguments)...));
  }

  static void SetLogger(std::shared_ptr<Logger>) noexcept;

};

}