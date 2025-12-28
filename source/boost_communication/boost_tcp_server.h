#pragma once

#include "../communication/tcp_server.h"
#include "boost/asio.hpp"
#include "boost_tcp_connection.h"

using boost::asio::ip::tcp;

namespace claw::communication {

class BoostTcpServer final : public TcpServer<chat::server::BoostTcpConnection> {
public:
  explicit BoostTcpServer(const std::string& ip, const unsigned short port)
    : acceptor_{io_context_, tcp::endpoint{boost::asio::ip::make_address(ip), port}}
  {}

  std::unique_ptr<chat::server::BoostTcpConnection> AcceptClientConnection() override;

private:
  boost::asio::io_context io_context_{};
  tcp::acceptor acceptor_;
};

}