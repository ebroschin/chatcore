#pragma once

#include "../communication/message_handler.h"
#include "../application/chat_tcp_system.h"

#include <claw/core/system.h>
#include <iostream>

namespace claw::chat::client {
class ChatInputSystem;
}

namespace claw::prototyping {

class ClientTestSystem final : public core::System {
public:
  explicit ClientTestSystem(const core::SystemContext& ctx);

  void Update() override;

private:
  chat::client::ChatInputSystem& chat_input_system_;
  chat::server::ChatClientTcpSystem& tcp_system_;
};

class PingMessageHandler final : public communication::MessageHandler {
public:
  void HandleMessage(const std::string& message) override {
    std::cout << "ping received: " << message << std::endl;
  }
};

}