#pragma once

#include "../communication/tcp_connector.h"
#include "boost/asio.hpp"
#include "boost_tcp_connection.h"

using boost::asio::ip::tcp;

namespace claw::communication {

struct BoostTcpServerParameters {
  std::string ip;
  unsigned short port;
};

class BoostTcpServer : public TcpConnector<chat::server::BoostTcpConnection, BoostTcpServerParameters> {
public:
  std::unique_ptr<chat::server::BoostTcpConnection> Connect(const BoostTcpServerParameters& parameters) override;

private:
  boost::asio::io_context io_context_{};

};

}