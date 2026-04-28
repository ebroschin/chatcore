#include "exit_command.hpp"

#include "../application/application_system.hpp"

#include <ebroschin/core/system_context.hpp>

namespace ebroschin::chatcore::client {

ExitCommand::ExitCommand(const core::SystemContext& ctx) noexcept:
  application_system_{ctx.Require<ApplicationSystem>()}
{}

void ExitCommand::Execute(std::span<std::string_view>) const {
  application_system_.Quit();
}

}
