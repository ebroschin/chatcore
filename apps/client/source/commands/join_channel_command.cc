#include "join_channel_command.h"
#include "../session/session_system.h"

namespace claw::chat::client {

JoinChannelCommand::JoinChannelCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>())
{ }

void JoinChannelCommand::Execute(std::span<std::string_view> arguments) const {
  if (arguments.size() < 1) return;

  const auto argument = std::string(arguments[0]);
  const auto channel_id = static_cast<api::PersistenceId>(std::stoul(argument));
  session_system_.JoinChannel(channel_id);
}

}