#pragma once

#include <span>
#include <string_view>
#include <claw/core/system_context.h>

namespace claw::chat::client {

class SessionSystem;

class AddChannelCommand {
public:
  static constexpr std::string_view Token = "addchannel";
  static constexpr std::string_view Description = "<channel_name> | Create a new channel";

  explicit AddChannelCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  SessionSystem& session_system_;
};

}