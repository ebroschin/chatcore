#pragma once

#include <string>
#include "../../communication/message_handler.h"

namespace claw::chat::server {

class ChatServerSystem;

class CreateChatChannelMessageHandler final : public communication::MessageHandler {
public:
  explicit CreateChatChannelMessageHandler(ChatServerSystem& system):
    system_{system}
  {}

  void HandleMessage(const std::string& message) override;

private:
  ChatServerSystem& system_;
};

}