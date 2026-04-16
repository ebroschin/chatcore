#pragma once

#include <ebroschin/core/system_context.h>
#include <span>
#include <string_view>

namespace ebroschin::chatcore::client {

class SessionSystem;

class LoginCommand {
public:
  static constexpr std::string_view Token = "login";
  static constexpr std::string_view Description = "<user_name> <password> | Authenticate and log in as a user";

  explicit LoginCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  SessionSystem& session_system_;
};

}