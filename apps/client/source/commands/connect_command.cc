#include "connect_command.h"

#include <claw/core/system_context.h>
#include "../model/model_system.h"
#include "../session/session_system.h"

namespace claw::chat::client {

ConnectCommand::ConnectCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>()),
  model_system_(ctx.Require<ModelSystem>())
{}

void ConnectCommand::Execute(std::span<std::string_view> arguments) const {
  if (arguments.size() < 2) return;

  const auto address = std::string(arguments[0]);
  const auto port = std::string(arguments[1]);
  session_system_.Connect(address, port);
  model_system_.AddLine("Connecting to " + address + ":" + port);
}

}