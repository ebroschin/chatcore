#pragma once

#include <string>

namespace claw::communication {

class MessageHandler {
public:
  virtual ~MessageHandler() = default;
  virtual void HandleMessage(const std::string& message) = 0;
};

}