#pragma once

#include <ebroschin/logging/logger.hpp>
#include <shared_mutex>
#include <sstream>

namespace claw::chat::client {

class UiSystem;
class ModelSystem;

class FtxuiLogger : public ebroschin::logging::Logger {
public:
  explicit FtxuiLogger(ModelSystem& model_system, UiSystem& ui_system) noexcept;

  void Print(ebroschin::logging::LogLevel log_level, const std::string& message) override;
  void SetLogLevel(ebroschin::logging::LogLevel log_level) override;
  void Shutdown() override;

private:
  ModelSystem& model_system_;
  UiSystem& ui_system_;
  std::mutex mutex_{};
  std::stringstream stream_{};
  ebroschin::logging::LogLevel log_level_{ebroschin::logging::LogLevel::info};
};

}