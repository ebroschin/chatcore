#pragma once

#include <string>
#include <optional>
#include <claw/utility/arguments.h>

namespace claw::chat::server {

class ChatServerArguments {
public:
  explicit ChatServerArguments(int argc, char** argv);

  [[nodiscard]] const std::string& GetIp() const noexcept
  { return ip_; }

  [[nodiscard]] unsigned short GetPort() const noexcept
  { return port_; }

  [[nodiscard]] const std::string& GetSqliteFilename() const noexcept
  { return sqlite_filename_; }

private:
  std::optional<std::string> ParseIp(const utility::Arguments& arguments);
  std::optional<unsigned short> ParsePort(const utility::Arguments& arguments);
  std::optional<std::string> ParseSqliteFilename(const utility::Arguments& arguments);

  std::string ip_;
  unsigned short port_;
  std::string sqlite_filename_;

};

}