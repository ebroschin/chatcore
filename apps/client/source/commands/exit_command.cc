#include "exit_command.h"

#include "../application/application_system.h"
#include <ebroschin/core/system_context.h>

namespace ebroschin::chatcore::client {

ExitCommand::ExitCommand(const core::SystemContext& ctx) noexcept:
  application_system_(ctx.Require<ApplicationSystem>())
{}

void ExitCommand::Execute(std::span<std::string_view>) const {
  application_system_.Quit();
}

}
