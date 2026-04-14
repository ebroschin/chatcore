#include "get_channels_command.h"

#include "../application/application_system.h"
#include "../session/session_system.h"
#include <claw/core/system_context.h>

namespace claw::chat::client {

GetChannelsCommand::GetChannelsCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>())
{}

void GetChannelsCommand::Execute(std::span<std::string_view>) const {
  session_system_.GetChannels();
}

}