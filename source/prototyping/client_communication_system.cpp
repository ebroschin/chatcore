#include "client_communication_system.h"

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
  std::string message;
  std::getline(std::cin, message);

  uint32_t network_length = htonl(message.length());
  boost::asio::write(socket_, boost::asio::buffer(&network_length, sizeof(network_length)));
  boost::asio::write(socket_, boost::asio::buffer(message));

  do {
    uint32_t network_response_length;
    boost::asio::read(socket_, boost::asio::buffer(&network_response_length, sizeof(network_response_length)));

    const uint32_t host_response_length = ntohl(network_response_length);
    std::vector<char> read_buffer(host_response_length);
    boost::asio::read(socket_, boost::asio::buffer(read_buffer));

    std::cout << "[message length] " << host_response_length << std::endl
    << "[message] " << std::string_view(read_buffer.data(), host_response_length) << std::endl;
  } while (socket_.available());

  // if (error == boost::asio::error::eof) {
  //   //app_.Quit();
  //   return;
  // }
  // else if (error) {
  //   app_.Quit();
  //   return;
  // }


}

}