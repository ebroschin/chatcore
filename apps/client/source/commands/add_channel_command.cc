#include "add_channel_command.h"

#include "../session/session_system.h"

namespace claw::chat::client {

AddChannelCommand::AddChannelCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>())
{ }

void AddChannelCommand::Execute(std::span<std::string_view> arguments) const {
  if (arguments.size() < 1) return;

  const auto channel_name = std::string(arguments[0]);
  session_system_.CreateChannel(channel_name);
}

}