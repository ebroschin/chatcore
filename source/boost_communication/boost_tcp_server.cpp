#include "boost_tcp_server.h"

#include <iostream>
#include <SQLiteCpp/SQLiteCpp.h>

namespace claw::communication {

void BoostTCPServer::Initialize() {
  std::cout << "server started" << std::endl;

  // Compile a SQL query, containing one parameter (index 1)
  db_.exec("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, value TEXT)");

  std::cout << "waiting for client" << std::endl;
  acceptor_.accept(socket_);
}

void BoostTCPServer::Update() {
  uint32_t network_length;
  boost::asio::read(socket_, boost::asio::buffer(&network_length, sizeof(network_length)));

  const uint32_t host_length = ntohl(network_length);
  std::vector<char> message_buffer(host_length);
  boost::asio::read(socket_, boost::asio::buffer(message_buffer));

  const std::string client_message{message_buffer.data(), host_length};

  std::cout << "client connected!" << std::endl;
  std::cout << "[RECEIVED] " << client_message << std::endl;

  const std::string response_message = "you wrote: " + client_message;
  uint32_t network_response_length = htonl(response_message.length());
  std::vector<char> write_buffer(network_response_length);

  boost::asio::write(socket_, boost::asio::buffer(&network_response_length, sizeof(network_response_length)));
  boost::asio::write(socket_, boost::asio::buffer(response_message.data(), response_message.length()));

  std::cout << "sent response" << std::endl;

  // // Insert a row
  // int nb = db_.exec("INSERT INTO test VALUES (NULL, 'test')");
  // std::cout << "INSERT INTO test VALUES (NULL, 'test'), returned " << nb << std::endl;
}

void BoostTCPServer::Deinitialize() {
  std::cout << "server shutdown" << std::endl;
}

}