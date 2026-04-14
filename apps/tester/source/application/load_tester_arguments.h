#pragma once

#include <string>
#include <optional>
#include <claw/utility/arguments.h>

namespace claw::chat::tester {

class LoadTesterArguments {
public:
  explicit LoadTesterArguments(int argc, char** argv);

  [[nodiscard]] const std::string& GetIp() const noexcept
  { return ip_; }

  [[nodiscard]] const std::string& GetPort() const noexcept
  { return port_; }

  [[nodiscard]] unsigned int GetClientCount() const noexcept
  { return client_count_; }

private:
  std::optional<std::string> ParseIp(const utility::Arguments& arguments);
  std::optional<std::string> ParsePort(const utility::Arguments& arguments);
  std::optional<unsigned int> ParseClientCount(const utility::Arguments& arguments);

  std::string ip_;
  std::string port_;
  unsigned int client_count_;
};

}