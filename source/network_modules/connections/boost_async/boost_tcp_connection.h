#pragma once
#include "claw/network/tcp/tcp_connection.h"

#include <boost/asio.hpp>
#include <queue>

namespace claw::network::modules {

class BoostTcpConnection final : public tcp::TcpConnection {
  using SendTask = std::function<std::span<const std::byte>>;
public:
  explicit BoostTcpConnection(boost::asio::ip::tcp::socket socket);

  void Start(ReceiverCallback callback, DisconnectCallback disconnect_callback) override;
  void SendBytes(std::span<const std::byte> bytes) override;

private:
  void SendNext();
  void ReadBytes();
  bool HandleError(const boost::system::error_code& error);

  boost::asio::ip::tcp::socket socket_;
  std::queue<std::vector<std::byte>> outgoing_bytes_{};

  std::uint32_t incoming_bytes_length_buffer_;
  std::vector<std::byte> incoming_bytes_buffer_{};
};

}