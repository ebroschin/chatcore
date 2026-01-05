#include "get_chat_messages_message_handler.h"

#include "../../communication/tcp_system.h"
#include "../chat_server_system.h"

namespace claw::chat::server {

void GetChatMessagesMessageHandler::HandleMessage(const std::string& message) {
  const std::int64_t channel_id = std::stoll(message);

  std::stringstream result;
  for(const std::string& message : system_.GetChatMessages(channel_id)) {
    result << message << "\n";
  }

  system_.GetTcpSystem().SendMessage<communication::TestMessage>({ result.str() });
}

}