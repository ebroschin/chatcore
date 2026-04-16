#include "add_channel_command.h"

#include "../session/session_system.h"

namespace ebroschin::chatcore::client {

AddChannelCommand::AddChannelCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>())
{ }

void AddChannelCommand::Execute(std::span<std::string_view> arguments) const {
  if (arguments.empty()) {
    ebroschin::logging::Log::Error() << Token << " requires argument <channel_name>";
    return;
  }

  const auto channel_name = std::string(arguments[0]);
  session_system_.CreateChannel(channel_name);
}

}