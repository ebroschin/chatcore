#pragma once

#include "../communication/tcp_server.h"
#include "boost/asio.hpp"

#include <SQLiteCpp/SQLiteCpp.h>

using boost::asio::ip::tcp;

namespace claw::communication {

class BoostTCPServer final : public TCPServer {
public:
  explicit BoostTCPServer(const std::string& ip, const unsigned short port)
    : acceptor_{io_context_, tcp::endpoint{boost::asio::ip::make_address(ip), port}}
  {}

  void Initialize() override;
  void Update() override;
  void Deinitialize() override;

private:
  void BufferResponse(const std::string& message);
  void SendResponse(const std::string& message);
  void ProcessMessage(const std::string& message);
  std::string ReadMessage();

  boost::asio::io_context io_context_{};
  tcp::acceptor acceptor_;
  tcp::socket socket_{io_context_};

  std::string response_message_buffer_{};
};

}