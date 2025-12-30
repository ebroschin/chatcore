#pragma once

#include "../communication/tcp_connector.h"
#include "boost/asio.hpp"
#include "boost_tcp_connection.h"

using boost::asio::ip::tcp;

namespace claw::communication {

struct BoostTcpClientParameters {
  std::string ip;
  std::string port;
};

class BoostTcpClient : public TcpConnector<chat::server::BoostTcpConnection, BoostTcpClientParameters> {
public:
  std::unique_ptr<chat::server::BoostTcpConnection> Connect(const BoostTcpClientParameters& parameters) override;

private:
  boost::asio::io_context io_context_{};
  tcp::resolver resolver_{io_context_};
};

}