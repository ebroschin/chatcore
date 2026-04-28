#pragma once

#include <ebroschin/logging/logger.hpp>

#include <mutex>
#include <sstream>

namespace ebroschin::chatcore::client {

class UiSystem;
class ModelSystem;

class FtxuiLogger : public logging::Logger {
public:
  explicit FtxuiLogger(ModelSystem& model_system, UiSystem& ui_system) noexcept;

  void Print(logging::LogLevel log_level, const std::string& message) override;
  void SetLogLevel(logging::LogLevel log_level) override;
  void Shutdown() override;

private:
  ModelSystem& model_system_;
  UiSystem& ui_system_;

  std::mutex mutex_{};
  std::stringstream stream_{};
  logging::LogLevel log_level_{logging::LogLevel::info};
};

}