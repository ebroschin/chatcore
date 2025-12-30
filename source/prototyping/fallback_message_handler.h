#pragma once

#include "../communication/message_handler.h"

#include <iostream>

namespace claw::prototyping {

class FallbackMessageHandler final : public communication::MessageHandler {
public:
  void HandleMessage(const std::string& message) override {
    std::cout << "[fallback]" << message << std::endl;
  }

};

}