#include "boost_tcp_connection.h"

namespace claw::chat::server {

void BoostTcpConnection::Start(ReceiverCallback callback) {
  callback_ = std::move(callback);
  worker_ = std::thread{&BoostTcpConnection::Poll, this};
}

void BoostTcpConnection::Poll() {
  while (true) {
    auto bytes = ReadMessage(); //needs to handle eof
    callback_(bytes);
  }
}

void BoostTcpConnection::SendMessage(std::span<const std::byte> bytes) {
  boost::system::error_code error;
  uint32_t network_length = htonl(bytes.size());
  boost::asio::write(socket_, boost::asio::buffer(&network_length, sizeof(network_length)), error);
  if (HandleError(error)) return;

  boost::asio::write(socket_, boost::asio::buffer(bytes), error);
  HandleError(error);
}

bool BoostTcpConnection::IsOpen() {
  return open_;
}

std::vector<std::byte> BoostTcpConnection::ReadMessage() {
  boost::system::error_code error;
  uint32_t network_length;
  boost::asio::read(socket_, boost::asio::buffer(&network_length, sizeof(network_length)), error);
  if (HandleError(error)) return {};

  const uint32_t host_length = ntohl(network_length);
  std::vector<std::byte> message_buffer(host_length);
  boost::asio::read(socket_, boost::asio::buffer(message_buffer), error);
  if (HandleError(error)) return {};

  return message_buffer;
}

bool BoostTcpConnection::HasData() {
  if (!IsOpen()) return false;

  boost::system::error_code error;
  const bool result = socket_.available(error);
  if (HandleError(error)) return false;

  return result;
}

bool BoostTcpConnection::HandleError(const boost::system::error_code& error) {
  if (!error) return false;

  open_ = false;
  return true;
}

}