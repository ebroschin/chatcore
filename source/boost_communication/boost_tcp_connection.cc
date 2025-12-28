#include "boost_tcp_connection.h"

namespace claw::chat::server {

void BoostTcpConnection::SendMessage(const std::string &message) {
  boost::system::error_code error;
  uint32_t network_length = htonl(message.length());
  boost::asio::write(socket_, boost::asio::buffer(&network_length, sizeof(network_length)), error);
  if (HandleError(error)) return;

  boost::asio::write(socket_, boost::asio::buffer(message), error);
  if (HandleError(error)) return;
}

std::string BoostTcpConnection::ReadMessage() {
  boost::system::error_code error;
  uint32_t network_length;
  boost::asio::read(socket_, boost::asio::buffer(&network_length, sizeof(network_length)), error);
  if (HandleError(error)) return {};

  const uint32_t host_length = ntohl(network_length);
  std::vector<char> message_buffer(host_length);
  boost::asio::read(socket_, boost::asio::buffer(message_buffer), error);
  if (HandleError(error)) return {};

  return std::string{message_buffer.data(), host_length};
}

bool BoostTcpConnection::HasData() {
  if (!IsOpen()) return false;

  boost::system::error_code error;
  const bool result = socket_.available(error);
  if (HandleError(error)) return false;

  return result;
}

bool BoostTcpConnection::IsOpen() {
  return open_;
}

bool BoostTcpConnection::HandleError(const boost::system::error_code& error) {
  if (!error) return false;

  open_ = false;
  return true;
}

}