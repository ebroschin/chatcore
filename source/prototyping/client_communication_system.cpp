#include "client_communication_system.h"

#include "chat_input_system.h"

#include <iostream>
#include <ranges>

namespace claw::communication {

void ClientCommunicationSystem::Initialize() {
  tcp::resolver resolver(io_context_);

  //const auto endpoints = resolver.resolve("192.168.178.20", "1338");
  const auto endpoints = resolver.resolve("localhost", "1338");
  boost::asio::connect(socket_, endpoints);
}

void ClientCommunicationSystem::Update() {

  std::string line;
  if (ctx_.Get<chat::client::ChatInputSystem>()->GetLine(line)) {
    SendMessage(line);
  }

  if (!socket_.available()) return;

  std::string response_message = ReadMessage();

  //process messages via message handler
  std::cout << "[RECEIVED] " << response_message << std::endl;
}

std::string ClientCommunicationSystem::ReadMessage() {
  uint32_t network_length;
  boost::asio::read(socket_, boost::asio::buffer(&network_length, sizeof(network_length)));

  const uint32_t host_length = ntohl(network_length);
  std::vector<char> message_buffer(host_length);
  boost::asio::read(socket_, boost::asio::buffer(message_buffer));

  return std::string{message_buffer.data(), host_length};
}

void ClientCommunicationSystem::SendMessage(const std::string& message) {
  uint32_t network_length = htonl(message.length());
  boost::asio::write(socket_, boost::asio::buffer(&network_length, sizeof(network_length)));
  boost::asio::write(socket_, boost::asio::buffer(message));
}

}