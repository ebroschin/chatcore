#pragma once

#include "chat_system_message_handler.h"
#include <string>

namespace claw::chat::server {

class ChatServerSystem;

class CreateChatChannelMessageHandler final : public ChatSystemMessageHandler {
public:
  explicit CreateChatChannelMessageHandler(ChatServerSystem& system):
    ChatSystemMessageHandler{system}
  {}

  void HandleMessage(const std::string& message) override;
};

}