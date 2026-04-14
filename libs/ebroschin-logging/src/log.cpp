#include <ebroschin/logging/log.hpp>

#include <ebroschin/logging/null_logger.hpp>
#include <mutex>
#include <shared_mutex>

namespace {
  std::shared_ptr<ebroschin::logging::Logger> logger_ = std::make_shared<ebroschin::logging::NullLogger>();
  std::shared_mutex mutex_{};
}

namespace ebroschin::logging {

void Log::SetLogger(std::shared_ptr<Logger> logger) noexcept {
  std::unique_lock lock(mutex_);
  logger_ = std::move(logger);
}

void Log::Verbose(const std::string& message) {
  Print(LogLevel::verbose, message);
}

void Log::Debug(const std::string& message) {
  Print(LogLevel::debug, message);
}

void Log::Info(const std::string& message) {
  Print(LogLevel::info, message);
}

void Log::Warning(const std::string& message) {
  Print(LogLevel::warning, message);
}

void Log::Error(const std::string& message) {
  Print(LogLevel::error, message);
}

void Log::Critical(const std::string& message) {
  Print(LogLevel::critical, message);
}

void Log::SetLogLevel(LogLevel log_level) {
  std::shared_ptr<Logger> logger;
  {
    std::shared_lock lock(mutex_);
    logger = logger_;
  }

  if (!logger) return;
  logger->SetLogLevel(log_level);
}

void Log::Print(LogLevel log_level, const std::string& message) {
  std::shared_ptr<Logger> logger;
  {
    std::shared_lock lock(mutex_);
    logger = logger_;
  }

  if (!logger) return;
  switch (log_level) {
    case LogLevel::verbose: logger->Verbose(message); break;
    case LogLevel::debug: logger->Debug(message); break;
    case LogLevel::info: logger->Info(message); break;
    case LogLevel::warning: logger->Warning(message); break;
    case LogLevel::error: logger->Error(message); break;
    case LogLevel::critical: logger->Critical(message); break;
  }
}

}