#include "client_communication_system.h"

#include <iostream>

namespace claw::communication {

void ClientCommunicationSystem::Initialize() {
  tcp::resolver resolver(io_context_);
  const auto endpoints = resolver.resolve("192.168.178.20", "1338");

  boost::asio::connect(socket_, endpoints);
}

void ClientCommunicationSystem::Update() {
  std::array<char, 128> buffer;
  boost::system::error_code error;

  const size_t length = socket_.read_some(boost::asio::buffer(buffer), error);
  if (error == boost::asio::error::eof) {
    app_.Quit();
    return;
  }
  else if (error) {
    app_.Quit();
    return;
  }

  std::cout << "[message length] " << length << "\n" << "[message] " << buffer.data() << std::endl;
}

void ClientCommunicationSystem::Quit() {

}

}