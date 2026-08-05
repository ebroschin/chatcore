#include "exit_command.hpp"

#include "../session/session_system.hpp"

#include <ebroschin/core/system_context.hpp>

namespace ebroschin::chatcore::client {

ExitCommand::ExitCommand(const core::SystemContext& ctx) noexcept:
  session_system_{ctx.Require<SessionSystem>()}
{}

void ExitCommand::Execute(std::span<std::string_view>) const {
  session_system_.Quit();
}

}
