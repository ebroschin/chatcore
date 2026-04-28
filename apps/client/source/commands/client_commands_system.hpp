#pragma once

#include "add_channel_command.hpp"
#include "add_user_command.hpp"
#include "connect_command.hpp"
#include "exit_command.hpp"
#include "get_channels_command.hpp"
#include "help_command.hpp"
#include "join_channel_command.hpp"
#include "login_command.hpp"
#include "logout_command.hpp"

#include <ebroschin/commands/commands_system.hpp>

namespace ebroschin::chatcore::client {

using ClientCommandsSystem = commands::CommandsSystem<
  ConnectCommand,
  LoginCommand,
  AddUserCommand,
  JoinChannelCommand,
  GetChannelsCommand,
  AddChannelCommand,
  LogoutCommand,
  ExitCommand,
  HelpCommand
>;

}