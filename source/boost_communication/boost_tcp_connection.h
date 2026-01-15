#pragma once

#include <claw/network/tcp/tcp_connection.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace claw::chat::server {

class BoostTcpConnection final : public network::tcp::TcpConnection {
public:
  explicit BoostTcpConnection(tcp::socket socket):
    socket_{std::move(socket)}
  {}

  ~BoostTcpConnection() override {
    running_ = false;
    if (!worker_.joinable()) return;
    worker_.join();
  }

  void Start(ReceiverCallback callback, DisconnectCallback disconnect_callback) override;
  void SendBytes(std::span<const std::byte> bytes) override;

private:
  void Poll();
  bool HandleError(const boost::system::error_code& error);
  void ReadBytes();
  void HandleDisconnect();

  bool running_{true};
  tcp::socket socket_;
  std::thread worker_{};
};

}