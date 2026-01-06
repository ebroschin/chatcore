#pragma once

#include "boost/asio.hpp"
#include "boost_tcp_connection.h"

using boost::asio::ip::tcp;

namespace claw::communication {

struct BoostTcpClientParameters {
  std::string ip;
  std::string port;
};

class BoostTcpClient {
public:
  using ConnectionType = chat::server::BoostTcpConnection;
  using ParameterType = BoostTcpClientParameters;

  std::unique_ptr<ConnectionType> Connect(const ParameterType& parameters);

private:
  boost::asio::io_context io_context_{};
  tcp::resolver resolver_{io_context_};
};

}