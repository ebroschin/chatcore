#include "boost_tcp_connection.h"

#include <iostream>

namespace claw::network::modules {

BoostTcpConnection::BoostTcpConnection(boost::asio::ip::tcp::socket socket):
  socket_{std::move(socket)}
{}

void BoostTcpConnection::Start(ReceiverCallback callback, DisconnectCallback disconnect_callback) {
  receiver_callback_ = callback;
  disconnect_callback_ = disconnect_callback;
  ReadBytes();
}

void BoostTcpConnection::SendBytes(std::span<const std::byte> bytes) {
  outgoing_bytes_.emplace(bytes.begin(), bytes.end());
  if (outgoing_bytes_.size() > 1) return;
  SendNext();
}

void BoostTcpConnection::SendNext() {
  if (outgoing_bytes_.empty()) return;

  auto bytes = std::move(outgoing_bytes_.front());
  outgoing_bytes_.pop();

  //TODO refactor this with futures or something, this is painful
  uint32_t network_length = htonl(static_cast<std::uint32_t>(bytes.size()));
  boost::asio::async_write(socket_, boost::asio::buffer(&network_length, sizeof(network_length)),
    [this, bytes](const boost::system::error_code& error, std::size_t)
  {
    if (HandleError(error)) return;
    boost::asio::async_write(socket_, boost::asio::buffer(bytes),
      [this](const boost::system::error_code& error, std::size_t)
    {
      if (HandleError(error)) return;
      SendNext();
    });
  });
}

void BoostTcpConnection::ReadBytes() {
  //TODO same here
  boost::asio::async_read(socket_, boost::asio::buffer(&incoming_bytes_length_buffer_, sizeof(incoming_bytes_length_buffer_)),
    [this](const boost::system::error_code& error, std::size_t) {
      if (HandleError(error)) return;

      auto host_length = ntohl(incoming_bytes_length_buffer_);
      incoming_bytes_buffer_.resize(host_length);

      boost::asio::async_read(socket_, boost::asio::buffer(incoming_bytes_buffer_),
      [this](const boost::system::error_code& error, std::size_t) {
        if (HandleError(error)) return;

        //TODO passes view, next read will invalidate memory, receiver must process immediately
        receiver_callback_(incoming_bytes_buffer_);
        ReadBytes();
      });
    });
}

bool BoostTcpConnection::HandleError(const boost::system::error_code& error) {
  if (!error) return false;

  if (error == boost::asio::error::eof) {
    std::cout << "connection closed by peer" << std::endl;
  }

  disconnect_callback_();

  return true;
}

}