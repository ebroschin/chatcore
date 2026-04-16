#include "logout_command.h"

#include "../application/application_system.h"
#include "../session/session_system.h"
#include "ebroschin/core/system_context.h"

namespace ebroschin::chatcore::client {

LogoutCommand::LogoutCommand(const core::SystemContext& ctx) noexcept:
  session_system_(ctx.Require<SessionSystem>())
{}

void LogoutCommand::Execute(std::span<std::string_view>) const {
  session_system_.Logout();
}

}