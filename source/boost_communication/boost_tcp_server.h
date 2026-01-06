#pragma once

#include "boost/asio.hpp"
#include "boost_tcp_connection.h"

using boost::asio::ip::tcp;

namespace claw::communication {

struct BoostTcpServerParameters {
  std::string ip;
  unsigned short port;
};

class BoostTcpServer {
public:
  using ConnectionType = chat::server::BoostTcpConnection;
  using ParameterType = BoostTcpServerParameters;

  std::unique_ptr<ConnectionType> Connect(const BoostTcpServerParameters& parameters);

private:
  boost::asio::io_context io_context_{};
};

}