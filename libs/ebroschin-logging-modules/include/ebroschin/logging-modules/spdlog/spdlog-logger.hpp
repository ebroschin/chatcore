#pragma once

#include <ebroschin/logging/logger.hpp>
#include <string>
#include <spdlog/spdlog.h>

namespace ebroschin::logging::modules {

class SpdlogLogger : public Logger {
public:
  explicit SpdlogLogger();

  void Verbose(const std::string& message) override;
  void Debug(const std::string& message) override;
  void Info(const std::string& message) override;
  void Warning(const std::string& message) override;
  void Error(const std::string& message) override;
  void Critical(const std::string& message) override;

  void SetLogLevel(LogLevel log_level) override;

private:
  std::shared_ptr<spdlog::logger> logger_{};
};

}