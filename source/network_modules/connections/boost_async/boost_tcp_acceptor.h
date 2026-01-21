#pragma once

#include "boost_tcp_connection.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <thread>

namespace claw::network::modules {

struct BoostTcpAcceptorParameters {
  std::string ip;
  unsigned short port;
};

class BoostTcpAcceptor {
public:
  using ConnectionType = BoostTcpConnection;
  using ParameterType = BoostTcpAcceptorParameters;
  using CallbackType = std::function<void(std::shared_ptr<ConnectionType>)>;

  ~BoostTcpAcceptor();

  void Connect(const ParameterType& parameters, CallbackType callback);

private:
  void StartAccept();
  void HandleAccept(const boost::system::error_code& error, boost::asio::ip::tcp::socket socket);

  //bool running_{false};
  boost::asio::io_context io_context_{};
  std::thread io_thread_{};
  std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_{};
  boost::asio::ip::tcp::socket pending_socket_{io_context_};
  CallbackType callback_{};
};

}