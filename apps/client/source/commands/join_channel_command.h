#pragma once

#include <span>
#include <string_view>
#include <claw/core/system_context.h>

namespace claw::chat::client {

class SessionSystem;

class JoinChannelCommand {
public:
  static constexpr std::string_view Token = "join";
  static constexpr std::string_view Description =
    "<channel_id> | Join a chat channel and receive its latest messages";

  explicit JoinChannelCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  SessionSystem& session_system_;
};

}