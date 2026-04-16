#include "chat_server_arguments.h"

#include <limits>

namespace ebroschin::chatcore::server {

ChatServerArguments::ChatServerArguments(int argc, char** argv) {
  const utility::Arguments arguments{argc, argv};

  ip_ = ParseIp(arguments).value_or("0.0.0.0");
  port_ = ParsePort(arguments).value_or(1338);
  sqlite_filename_ = ParseSqliteFilename(arguments).value_or("sqlite.db");
  log_level_ = ParseLogLevel(arguments).value_or(logging::LogLevel::info);
}

std::optional<std::string> ChatServerArguments::ParseIp(const utility::Arguments& arguments) {
  const auto values = arguments.GetValues("ip");
  if (!values || values->empty()) return std::nullopt;

  return values->front();
}

std::optional<unsigned short> ChatServerArguments::ParsePort(const utility::Arguments& arguments) {
  const auto values = arguments.GetValues("port");
  if (!values || values->empty()) return std::nullopt;

  try {
    const auto result = std::stoul(values->front());
    if (result > std::numeric_limits<unsigned short>::max()) return std::nullopt;

    return static_cast<unsigned short>(result);
  } catch (const std::exception&) {
    return std::nullopt;
  }
}

std::optional<std::string> ChatServerArguments::ParseSqliteFilename(const utility::Arguments& arguments) {
  const auto values = arguments.GetValues("db");
  if (!values || values->empty()) return std::nullopt;

  return values->front();
}

std::optional<logging::LogLevel> ChatServerArguments::ParseLogLevel(const utility::Arguments& arguments) {
  const auto values = arguments.GetValues("log");
  if (!values || values->empty()) return std::nullopt;

  const auto result = values->front();
  if (result == "verbose") return logging::LogLevel::verbose;
  if (result == "debug") return logging::LogLevel::debug;
  if (result == "info") return logging::LogLevel::info;
  if (result == "warning") return logging::LogLevel::warning;
  if (result == "error") return logging::LogLevel::error;
  if (result == "critical") return logging::LogLevel::critical;

  return std::nullopt;
}

}