#include "login_command.hpp"

#include "../application/application_system.hpp"
#include "../session/session_system.hpp"

#include <ebroschin/core/system_context.hpp>

namespace ebroschin::chatcore::client {

LoginCommand::LoginCommand(const core::SystemContext& ctx) noexcept:
  session_system_{ctx.Require<SessionSystem>()}
{}

void LoginCommand::Execute(std::span<std::string_view> arguments) const {
  if (arguments.size() < 2) {
    logging::Log::Error() << Token << " requires 2 arguments <user_name> <password>";
    return;
  }

  auto name = std::string{arguments[0]};
  auto password = std::string{arguments[1]};
  session_system_.Login(std::move(name), std::move(password));
}

}