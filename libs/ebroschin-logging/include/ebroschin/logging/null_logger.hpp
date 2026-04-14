#pragma once

#include "logger.hpp"

namespace ebroschin::logging {

class NullLogger : public Logger {
public:
  void Verbose(const std::string& message) override { }
  void Debug(const std::string& message) override { }
  void Info(const std::string& message) override { }
  void Warning(const std::string& message) override { }
  void Error(const std::string& message) override { }
  void Critical(const std::string& message) override { }
  void SetLogLevel(LogLevel log_level) override { }
};

}