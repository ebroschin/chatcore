#pragma once

#include "add_channel_command.h"
#include "add_user_command.h"
#include "commands_system.h"
#include "connect_command.h"
#include "get_channels_command.h"
#include "join_channel_command.h"
#include "login_command.h"
#include "logout_command.h"
#include "help_command.h"

namespace claw::chat::client {

using ClientCommandsSystem = commands::CommandsSystem<
  ConnectCommand,
  LoginCommand,
  AddUserCommand,
  JoinChannelCommand,
  GetChannelsCommand,
  AddChannelCommand,
  LogoutCommand,
  HelpCommand
>;

}