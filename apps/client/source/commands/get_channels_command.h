#pragma once

#include <string_view>
#include <claw/core/system_context.h>
#include <span>

namespace claw::chat::client {

class SessionSystem;

class GetChannelsCommand {
public:
  static constexpr std::string_view Token = "channels";
  static constexpr std::string_view Description = "| Print all channels and their ids";

  explicit GetChannelsCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  SessionSystem& session_system_;
};

}