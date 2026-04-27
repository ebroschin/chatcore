#include "add_channel_command.hpp"

#include "../session/session_system.hpp"

namespace ebroschin::chatcore::client {

AddChannelCommand::AddChannelCommand(const core::SystemContext& ctx) noexcept:
  session_system_{ctx.Require<SessionSystem>()}
{ }

void AddChannelCommand::Execute(std::span<std::string_view> arguments) const {
  if (arguments.empty()) {
    logging::Log::Error() << Token << " requires argument <channel_name>";
    return;
  }

  auto channel_name = std::string{arguments[0]};
  session_system_.CreateChannel(std::move(channel_name));
}

}