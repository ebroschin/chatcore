#include "boost_tcp_connection.h"

#include <iostream>

namespace claw::chat::server {

void BoostTcpConnection::Start(ReceiverCallback callback, DisconnectCallback disconnect_callback) {
  receiver_callback_ = std::move(callback);
  disconnect_callback_ = std::move(disconnect_callback);
  worker_ = std::thread{&BoostTcpConnection::Poll, this};
}

void BoostTcpConnection::Poll() {
  while (running_) {
    ReadBytes();
  }
}

void BoostTcpConnection::SendBytes(std::span<const std::byte> bytes) {
  boost::system::error_code error;
  uint32_t network_length = htonl(static_cast<std::uint32_t>(bytes.size()));
  boost::asio::write(socket_, boost::asio::buffer(&network_length, sizeof(network_length)), error);
  if (HandleError(error)) return;

  boost::asio::write(socket_, boost::asio::buffer(bytes), error);
  HandleError(error);
}

void BoostTcpConnection::ReadBytes() {
  boost::system::error_code error;
  uint32_t network_length;
  boost::asio::read(socket_, boost::asio::buffer(&network_length, sizeof(network_length)), error);
  if (HandleError(error)) return;

  const uint32_t host_length = ntohl(network_length);
  std::vector<std::byte> message_buffer(host_length);
  boost::asio::read(socket_, boost::asio::buffer(message_buffer), error);
  if (HandleError(error)) return;

  task_thread_.Post([this, message_buffer]() {
    receiver_callback_(message_buffer);
  });
}

bool BoostTcpConnection::HandleError(const boost::system::error_code& error) {
  if (!error) return false;

  if (error == boost::asio::error::eof) {
    std::cout << "connection closed by peer" << std::endl;
  }

  running_ = false;
  task_thread_.Post([this]() {
    disconnect_callback_();
  });

  return true;
}

}