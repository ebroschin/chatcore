#pragma once

#include <claw/network/tcp/tcp_connection.h>
#include <boost/asio.hpp>
#include <claw/utility/task_thread.h>

namespace claw::network::modules {

class SyncBoostTcpConnection final : public tcp::TcpConnection {
public:
  explicit SyncBoostTcpConnection(boost::asio::ip::tcp::socket socket, utility::TaskThread& task_thread):
    socket_{std::move(socket)},
    task_thread_{task_thread}
  {}

  ~SyncBoostTcpConnection() override {
    running_ = false;
    if (!worker_.joinable()) return;
    worker_.join();
  }

  void Start() override;
  void Disconnect() override {}
  void SendBytes(std::span<const std::byte> bytes) override;

private:
  void Poll();
  bool HandleError(const boost::system::error_code& error);
  void ReadBytes();
  void HandleDisconnect();

  bool running_{true};
  boost::asio::ip::tcp::socket socket_;
  std::thread worker_{};

  utility::TaskThread& task_thread_;
};

}