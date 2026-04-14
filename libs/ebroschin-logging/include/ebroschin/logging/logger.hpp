#pragma once

#include <string>
#include "log_level.hpp"

namespace ebroschin::logging {

class Logger {
public:
  virtual ~Logger() = default;

  virtual void Verbose(const std::string& message) = 0;
  virtual void Debug(const std::string& message) = 0;
  virtual void Info(const std::string& message) = 0;
  virtual void Warning(const std::string& message) = 0;
  virtual void Error(const std::string& message) = 0;
  virtual void Critical(const std::string& message) = 0;

  virtual void SetLogLevel(LogLevel log_level) = 0;
};

}