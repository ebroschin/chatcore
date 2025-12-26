#include "boost_tcp_server.h"

#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <ranges>

namespace claw::communication {

void BoostTCPServer::Initialize() {
  std::cout << "server started" << std::endl;

  // Compile a SQL query, containing one parameter (index 1)
  db_.exec("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, value TEXT)");

  std::cout << "waiting for client" << std::endl;
  acceptor_.accept(socket_);
  std::cout << "client connected!" << std::endl;
}

void BoostTCPServer::Update() {

  std::string message = ReadMessage();
  ProcessMessage(message);
  BufferResponse("you wrote: " + message);
  SendResponse(response_message_buffer_);
}

void BoostTCPServer::Deinitialize() {
  std::cout << "server shutdown" << std::endl;
}

void BoostTCPServer::BufferResponse(const std::string &message) {
  response_message_buffer_ += message;
}

void BoostTCPServer::SendResponse(const std::string& message) {
  uint32_t network_response_length = htonl(message.length());
  std::vector<char> write_buffer(network_response_length);

  boost::asio::write(socket_, boost::asio::buffer(&network_response_length, sizeof(network_response_length)));
  boost::asio::write(socket_, boost::asio::buffer(message.data(), message.length()));

  std::cout << "[SENT] " << message << std::endl;
  response_message_buffer_.clear();
}

void BoostTCPServer::ProcessMessage(const std::string& message) {
  std::cout << "[RECEIVED] " << message << std::endl;

  if (message.starts_with("write ")) {
    const std::string content{std::string_view(message) | std::views::drop(std::strlen("write "))};

    int nb = db_.exec("INSERT INTO test VALUES (NULL, '" + content + "')");
    BufferResponse("modified " + std::to_string(nb) + " rows");
    return;
  }

  if (message.starts_with("get")) {
    SQLite::Statement query(db_, "SELECT * FROM test");

    std::stringstream string_stream;
    while (query.executeStep()) {
      for (int i = 0; i < query.getColumnCount(); i++) {
        string_stream << query.getColumn(i).getString() << "\n";
      }
    }

    BufferResponse(string_stream.str());
    return;
  }
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