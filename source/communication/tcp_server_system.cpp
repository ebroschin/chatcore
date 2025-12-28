#include "tcp_server_system.h"

#include <ranges>

namespace claw::communication {

void TcpServerSystemBase::HandleMessage(const std::string &message) {
  //TODO protocol implementation details
  const auto index = message.find_first_of(':');
  const std::string message_type = message.substr(0, index);
  const std::string content{std::string_view(message) | std::views::drop(index + 1)};

  message_handler_registry_.HandleMessage(message_type, content);
}

void TcpServerSystemBase::SendMessage(const std::string& message) {
  if (connection_ == nullptr) return;
  connection_->SendMessage(message);
}

void TcpServerSystemBase::Initialize() {
  std::cout << "starting server" << std::endl;
}

void TcpServerSystemBase::Update() {
  Connect();
  if (connection_ == nullptr) return;
  
  if (connection_->HasData()) {
    HandleMessage(connection_->ReadMessage());
  }
}

}