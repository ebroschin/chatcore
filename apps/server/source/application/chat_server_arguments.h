#pragma once

#include <ebroschin/logging/log_level.hpp>
#include <ebroschin/utility/arguments.h>
#include <optional>
#include <string>

namespace ebroschin::chatcore::server {

class ChatServerArguments {
public:
  explicit ChatServerArguments(int argc, char** argv);

  [[nodiscard]] const std::string& GetIp() const noexcept
  { return ip_; }

  [[nodiscard]] unsigned short GetPort() const noexcept
  { return port_; }

  [[nodiscard]] const std::string& GetSqliteFilename() const noexcept
  { return sqlite_filename_; }

  [[nodiscard]] logging::LogLevel GetLogLevel() const noexcept
  { return log_level_; }

private:
  std::optional<std::string> ParseIp(const utility::Arguments& arguments);
  std::optional<unsigned short> ParsePort(const utility::Arguments& arguments);
  std::optional<std::string> ParseSqliteFilename(const utility::Arguments& arguments);
  std::optional<logging::LogLevel> ParseLogLevel(const utility::Arguments& arguments);

  std::string ip_;
  unsigned short port_;
  std::string sqlite_filename_;
  logging::LogLevel log_level_;

};

}