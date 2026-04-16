#include "help_command.h"

#include <ebroschin/core/system_context.h>
#include <ebroschin/utility/variadic.h>

#include "../application/application_system.h"
#include "client_commands_system.h"

namespace ebroschin::chatcore::client {

HelpCommand::HelpCommand(const core::SystemContext& ctx) noexcept:
  model_system_(ctx.Require<ModelSystem>())
{}

void HelpCommand::Execute(std::span<std::string_view>) {
  utility::ForEachType<ClientCommandsSystem::Commands>::Apply([this]<typename TCommand>() {
    const auto line = "/" + std::string{TCommand::Token} + " " + std::string{TCommand::Description};
    model_system_.AddLine(line);
  });
}

}