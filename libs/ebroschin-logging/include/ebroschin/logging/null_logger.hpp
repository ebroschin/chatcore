#pragma once

#include "logger.hpp"

namespace ebroschin::logging {

class NullLogger : public Logger {
public:
  void Print(LogLevel log_level, const std::string& message) override { }
  void SetLogLevel(LogLevel log_level) override { }
  void Shutdown() override { }
};

}