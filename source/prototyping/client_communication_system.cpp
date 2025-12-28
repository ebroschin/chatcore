#include "client_communication_system.h"

#include "chat_input_system.h"

#include <iostream>
#include <ranges>

namespace claw::communication {

void ClientCommunicationSystem::Initialize() {
  tcp::resolver resolver(io_context_);

  //const auto endpoints = resolver.resolve("192.168.178.20", "1338");
  const auto endpoints = resolver.resolve("localhost", "1338");
  tcp::socket socket{io_context_};
  boost::asio::connect(socket, endpoints);
  connection_ = std::make_unique<chat::server::BoostTcpConnection>(std::move(socket));
}

void ClientCommunicationSystem::Update() {
  if (connection_ == nullptr) return;

  std::string line;
  if (ctx_.Get<chat::client::ChatInputSystem>()->GetLine(line)) {
    if (connection_ != nullptr) {
      connection_->SendMessage(line);
    }
  }

  if (connection_->HasData()) return;
  std::string response_message = connection_->ReadMessage();

  //process messages via message handler
  std::cout << "[RECEIVED] " << response_message << std::endl;
}

void ClientCommunicationSystem::SendMessage(const std::string& message) {
  if (connection_ == nullptr) return;
  connection_->SendMessage(message);
}

}