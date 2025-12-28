#pragma once

#include "../../communication/message_handler.h"

namespace claw::chat::server {

class ChatServerSystem;

class ChatSystemMessageHandler : public communication::MessageHandler {
public:
  explicit ChatSystemMessageHandler(ChatServerSystem& system):
    system_{system}
  {}

protected:
  ChatServerSystem& system_;
};

}