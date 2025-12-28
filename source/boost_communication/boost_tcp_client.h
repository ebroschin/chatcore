#pragma once

#include "../communication/tcp_connector.h"
#include "boost/asio.hpp"
#include "boost_tcp_connection.h"

using boost::asio::ip::tcp;

namespace claw::communication {

class BoostTcpClient final : public TcpConnector<chat::server::BoostTcpConnection> {
public:
  explicit BoostTcpClient(const std::string& ip, const unsigned short port)
    : ip_{ip}, port_{port}
  {}

  std::unique_ptr<chat::server::BoostTcpConnection> Connect() override;

private:
  std::string ip_;
  unsigned short port_;
  boost::asio::io_context io_context_{};
  tcp::resolver resolver_{io_context_};
};

}