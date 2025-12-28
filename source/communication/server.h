#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace claw::communication {

class MessageHandler {
public:
  virtual ~MessageHandler() = default;
  virtual void HandleMessage(const std::string& message) = 0;
};

class Server {
public:
  virtual ~Server() = default;

  template<typename TMessageHandler>
  requires std::derived_from<TMessageHandler, MessageHandler>
  void Register(const std::string& message_type) {
    handlers_.emplace(message_type, std::make_unique<TMessageHandler>());
  }

protected:
  void Handle(const std::string& message_type, const std::string& message) {
    const auto it = handlers_.find(message_type);
    if (it == handlers_.end()) return;

    it->second->HandleMessage(message);
  }

private:
  std::unordered_map<std::string, std::unique_ptr<MessageHandler>> handlers_;
};

}