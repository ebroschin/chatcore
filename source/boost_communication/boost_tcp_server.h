#pragma once

#include "boost/asio.hpp"
#include "boost_tcp_connection.h"

#include <memory>

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
  using CallbackType = std::function<void(std::shared_ptr<ConnectionType>)>;

  ~BoostTcpServer();

  void Connect(const BoostTcpServerParameters& parameters, CallbackType callback);

private:
  void Accept(const ParameterType& parameters, CallbackType callback);

  boost::asio::io_context io_context_{};
  std::thread worker_{};
};

}