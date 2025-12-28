#pragma once

#include <string>
#include "../../communication/message_handler.h"

namespace claw::chat::server {

class ChatServerSystem;

class CreateChatMessageMessageHandler final : public communication::MessageHandler {
public:
  explicit CreateChatMessageMessageHandler(ChatServerSystem& system):
    system_{system}
  {}

  void HandleMessage(const std::string& message) override;

private:
  ChatServerSystem& system_;
};

}