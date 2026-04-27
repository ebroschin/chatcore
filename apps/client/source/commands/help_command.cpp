#include "help_command.hpp"

#include "../application/application_system.hpp"
#include "client_commands_system.hpp"

#include <ebroschin/core/system_context.hpp>
#include <ebroschin/utility/variadic.hpp>

namespace ebroschin::chatcore::client {

HelpCommand::HelpCommand(const core::SystemContext& ctx) noexcept:
  model_system_{ctx.Require<ModelSystem>()}
{}

void HelpCommand::Execute(std::span<std::string_view>) const {
  utility::ForEachType<ClientCommandsSystem::Commands>::Apply([this]<typename TCommand>() {
    const auto line = "/" + std::string{TCommand::Token} + " " + std::string{TCommand::Description};
    model_system_.AddLine(line);
  });
}

}