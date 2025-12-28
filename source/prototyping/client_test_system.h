#pragma once

#include "../communication/message_handler.h"

#include <claw/core/system.h>
#include <iostream>

namespace claw::prototyping {

class ClientTestSystem final : public core::System {
public:
  explicit ClientTestSystem(const core::SystemContext& ctx);
};

class PingMessageHandler final : public communication::MessageHandler {
public:
  void HandleMessage(const std::string& message) override {
    std::cout << "ping received: " << message << std::endl;
  }
};

}