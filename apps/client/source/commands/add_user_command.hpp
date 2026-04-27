#pragma once

#include <ebroschin/core/system_context.hpp>
#include <span>
#include <string_view>

namespace ebroschin::chatcore::client {

class SessionSystem;

class AddUserCommand {
public:
  static constexpr std::string_view Token = "adduser";
  static constexpr std::string_view Description = "<user_name> <password> | Create a new user";

  explicit AddUserCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  SessionSystem& session_system_;
};

}