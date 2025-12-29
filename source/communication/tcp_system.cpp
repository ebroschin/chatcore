#include "tcp_system.h"

#include <ranges>

namespace claw::communication {

void TcpSystem::HandleMessage(const std::string &message) {
  //TODO serialization implementation details => DTO's later
  const auto index = message.find_first_of(':');
  const std::string message_type = message.substr(0, index);
  const std::string content{std::string_view(message) | std::views::drop(index + 1)};

  message_handler_registry_.HandleMessage(message_type, content);
}

void TcpSystem::SendMessage(const std::string& message) {
  if (connection_ == nullptr) return;
  connection_->SendMessage(message);
}

void TcpSystem::Update() {
  Connect();
  if (connection_ == nullptr) return;

  if (connection_->HasData()) {
    HandleMessage(connection_->ReadMessage());
  }
}

}