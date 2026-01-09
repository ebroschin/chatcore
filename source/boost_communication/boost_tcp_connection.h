#pragma once
#include "../communication/tcp_connection.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace claw::chat::server {

class BoostTcpConnection final : public communication::TcpConnection {
public:
  explicit BoostTcpConnection(tcp::socket socket):
    socket_{std::move(socket)}
  {}

  ~BoostTcpConnection() override {
    if (!worker_.joinable()) return;
    worker_.join();
  }

  void Start(ReceiverCallback callback) override;
  void SendBytes(std::span<const std::byte> bytes) override;
  bool IsOpen() override;

private:
  void Poll();
  bool HandleError(const boost::system::error_code& error);
  bool HasData();
  std::vector<std::byte> ReadBytes();

  tcp::socket socket_;
  bool open_{true};
  std::thread worker_{};
};

}