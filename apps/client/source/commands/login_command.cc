#include "login_command.h"

#include "../application/application_system.h"
#include "../session/session_system.h"
#include "claw/core/system_context.h"

namespace claw::chat::client {

LoginCommand::LoginCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>())
{}

void LoginCommand::Execute(std::span<std::string_view> arguments) const {
  if (arguments.size() < 2) {
    ebroschin::logging::Log::Error() << Token << " requires 2 arguments <user_name> <password>";
    return;
  }

  const auto name = std::string(arguments[0]);
  const auto password = std::string(arguments[1]);
  session_system_.Login(name, password);
}

}