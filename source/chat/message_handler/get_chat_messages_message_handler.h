#pragma once
#include "chat_system_message_handler.h"

namespace claw::chat::server {

class GetChatMessagesMessageHandler final : public ChatSystemMessageHandler {
public:
  explicit GetChatMessagesMessageHandler(ChatServerSystem& system):
    ChatSystemMessageHandler(system)
  {}

  void HandleMessage(const std::string &message) override;
};

}