#include "connect_command.h"

#include "../model/model_system.h"
#include "../session/session_system.h"
#include <ebroschin/core/system_context.h>

namespace ebroschin::chatcore::client {

ConnectCommand::ConnectCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>()),
  model_system_(ctx.Require<ModelSystem>())
{ }

void ConnectCommand::Execute(std::span<std::string_view> arguments) const {
  if (arguments.size() < 2) {
    ebroschin::logging::Log::Error() << Token << " requires 2 arguments <address> <port>";
    return;
  }

  const auto address = std::string(arguments[0]);
  const auto port = std::string(arguments[1]);
  session_system_.Connect(address, port);
  ebroschin::logging::Log::Info() << "Connecting to " << address << ":" << port;
}

}