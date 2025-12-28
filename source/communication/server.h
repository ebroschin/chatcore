#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "message_handler.h"

namespace claw::communication {

class Server {
public:
  virtual ~Server() = default;

  template<typename TMessageHandler, typename... TArgs>
  requires std::derived_from<TMessageHandler, MessageHandler>
  void Register(const std::string& message_type, TArgs&&... args) {
    handlers_.emplace(message_type, std::make_unique<TMessageHandler>(std::forward<TArgs>(args)...));
  }

protected:
  void HandleMessage(const std::string& message_type, const std::string& message);

private:
  std::unordered_map<std::string, std::unique_ptr<MessageHandler>> handlers_;
};

}