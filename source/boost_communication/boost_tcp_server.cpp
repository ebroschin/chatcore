#include "boost_tcp_server.h"

#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <ranges>

namespace claw::communication {

void BoostTCPServer::Initialize() {
  std::cout << "server started" << std::endl;
}

void BoostTCPServer::Update() {
  if (!socket_.is_open()) {
    std::cout << "waiting for client" << std::endl;
    acceptor_.accept(socket_);
    std::cout << "client connected!" << std::endl;
  }

  if (socket_.available()) {
    const std::string message = ReadMessage();
    ProcessMessage(message);
    SendMessage("you wrote: " + message);
  }

  SendResponse(response_message_buffer_);
}

void BoostTCPServer::Deinitialize() {
  std::cout << "server shutdown" << std::endl;
}

void BoostTCPServer::SendMessage(const std::string &message) {
  response_message_buffer_ += message + "\n";
}

void BoostTCPServer::SendResponse(const std::string& message) {
  if (response_message_buffer_.empty()) return;

  boost::system::error_code error;
  uint32_t network_length = htonl(message.length());
  boost::asio::write(socket_, boost::asio::buffer(&network_length, sizeof(network_length)), error);
  if (error) {
    socket_.close();
    return;
  }

  boost::asio::write(socket_, boost::asio::buffer(message), error);
  if (error) return;

  std::cout << "[SENT] " << message << std::endl;
  response_message_buffer_.clear();
}

void BoostTCPServer::ProcessMessage(const std::string& message) {
  std::cout << "[RECEIVED] " << message << std::endl;

  const auto index = message.find_first_of(':');
  const std::string message_type = message.substr(0, index);
  const std::string content{std::string_view(message) | std::views::drop(index + 1)};

  HandleMessage(message_type, content);
}

std::string BoostTCPServer::ReadMessage() {
  uint32_t network_length;
  boost::asio::read(socket_, boost::asio::buffer(&network_length, sizeof(network_length)));

  const uint32_t host_length = ntohl(network_length);
  std::vector<char> message_buffer(host_length);
  boost::asio::read(socket_, boost::asio::buffer(message_buffer));

  return std::string{message_buffer.data(), host_length};
}

}