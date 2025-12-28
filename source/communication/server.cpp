#include "server.h"
#include "message_handler.h"

namespace claw::communication {

void Server::HandleMessage(const std::string& message_type, const std::string& message) {
  const auto it = handlers_.find(message_type);
  if (it == handlers_.end()) return;

  it->second->HandleMessage(message);
}

}