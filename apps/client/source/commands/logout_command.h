#pragma once

#include <claw/core/system_context.h>
#include <span>
#include <string_view>

namespace claw::chat::client {

class SessionSystem;

class LogoutCommand {
public:
  static constexpr std::string_view Token = "logout";
  static constexpr std::string_view Description = "| Log out as current user";

  explicit LogoutCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  SessionSystem& session_system_;
};

}