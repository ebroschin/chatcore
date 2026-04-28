#include "ftxui_logger.hpp"

#include "../model/model_system.hpp"
#include "ui_system.hpp"

#include <mutex>
#include <sstream>

namespace ebroschin::chatcore::client {

FtxuiLogger::FtxuiLogger(ModelSystem& model_system, UiSystem& ui_system) noexcept:
  model_system_{model_system},
  ui_system_{ui_system}
{ }

void FtxuiLogger::Print(logging::LogLevel log_level, const std::string& message) {
  std::string result{};
  {
    std::scoped_lock lock{mutex_};
    if (log_level < log_level_) return;

    stream_ << "[Log::" << ebroschin::logging::ToString(log_level) << "] " << message;
    result = stream_.str();
    stream_.str("");
    stream_.clear();
  }

  model_system_.AddLine(result);
}

void FtxuiLogger::SetLogLevel(logging::LogLevel log_level) {
  std::scoped_lock lock{mutex_};
  if (log_level > log_level_) return;

  log_level_ = log_level;
}

void FtxuiLogger::Shutdown() {
  std::scoped_lock lock{mutex_};
  ui_system_.Shutdown();
}

}