#pragma once
#include "../communication/tcp_connection.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace claw::chat::server {

class BoostTcpConnection final : public communication::TcpConnection {
public:
  explicit BoostTcpConnection(tcp::socket&& socket):
    socket_{std::move(socket)}
  {}

  void SendMessage(std::span<const std::byte> bytes) override;
  std::vector<std::byte> ReadMessage() override;
  bool HasData() override;
  bool IsOpen() override;

private:
  bool HandleError(const boost::system::error_code& error);

  tcp::socket socket_;
  bool open_{true};
};

}