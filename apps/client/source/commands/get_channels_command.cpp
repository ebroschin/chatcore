#include "get_channels_command.hpp"

#include "../application/application_system.hpp"
#include "../session/session_system.hpp"
#include <ebroschin/core/system_context.hpp>

namespace ebroschin::chatcore::client {

GetChannelsCommand::GetChannelsCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>())
{}

void GetChannelsCommand::Execute(std::span<std::string_view>) const {
  session_system_.GetChannels();
}

}