#pragma once

#include <ebroschin/logging/log_level.hpp>
#include <ebroschin/utility/arguments.h>
#include <optional>
#include <string>

namespace ebroschin::chatcore::tester {

class LoadTesterArguments {
public:
  explicit LoadTesterArguments(int argc, char** argv);

  [[nodiscard]] const std::string& GetIp() const noexcept
  { return ip_; }

  [[nodiscard]] const std::string& GetPort() const noexcept
  { return port_; }

  [[nodiscard]] unsigned int GetClientCount() const noexcept
  { return client_count_; }

  [[nodiscard]] logging::LogLevel GetLogLevel() const noexcept
  { return log_level_; }

private:
  std::optional<std::string> ParseIp(const utility::Arguments& arguments);
  std::optional<std::string> ParsePort(const utility::Arguments& arguments);
  std::optional<unsigned int> ParseClientCount(const utility::Arguments& arguments);
  std::optional<logging::LogLevel> ParseLogLevel(const utility::Arguments& arguments);

  std::string ip_;
  std::string port_;
  unsigned int client_count_;
  logging::LogLevel log_level_;

};

}