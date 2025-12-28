#include "create_chat_message_message_handler.h"

#include <ranges>
#include "../chat_server_system.h"

namespace claw::chat::server {

void CreateChatMessageMessageHandler::HandleMessage(const std::string& message) {
  const auto index = message.find_first_of(':');
  const std::int64_t channel_id = std::stoll(message.substr(0, index));
  const std::string chat_message{std::string_view(message) | std::views::drop(index + 1)};

  system_.CreateChatMessage(channel_id, chat_message);
}

}