#include "add_user_command.hpp"

#include "../application/application_system.hpp"
#include "../session/session_system.hpp"
#include <ebroschin/core/system_context.hpp>

namespace ebroschin::chatcore::client {

AddUserCommand::AddUserCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>())
{}

void AddUserCommand::Execute(std::span<std::string_view> arguments) const {
  if (arguments.size() < 2) {
    logging::Log::Error() << Token << " requires 2 arguments <user_name> <password>";
    return;
  }

  const auto name = std::string(arguments[0]);
  const auto password = std::string(arguments[1]);
  session_system_.AddUser(name, password);
}

}