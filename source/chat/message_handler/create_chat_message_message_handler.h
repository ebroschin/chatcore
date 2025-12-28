#pragma once

#include "chat_system_message_handler.h"
#include <string>

namespace claw::chat::server {

class ChatServerSystem;

class CreateChatMessageMessageHandler final : public ChatSystemMessageHandler {
public:
  explicit CreateChatMessageMessageHandler(ChatServerSystem& system):
    ChatSystemMessageHandler(system)
  {}

  void HandleMessage(const std::string& message) override;
};

}