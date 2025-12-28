#include "message_handler_registry.h"

namespace claw::communication {

void MessageHandlerRegistry::HandleMessage(const std::string& message_type, const std::string& message) {
  const auto it = handlers_.find(message_type);
  if (it == handlers_.end()) return;

  it->second->HandleMessage(message);
}

}