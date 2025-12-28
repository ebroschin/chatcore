#include "create_chat_channel_message_handler.h"

#include <ranges>
#include "../chat_server_system.h"

namespace claw::chat::server {

void CreateChatChannelMessageHandler::HandleMessage(const std::string& message)  {
  system_.CreateChatChannel(message);
}

}