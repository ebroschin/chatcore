#include "boost_tcp_server.h"

#include <ranges>
#include <iostream>

namespace claw::communication {

std::unique_ptr<BoostTcpServer::ConnectionType> BoostTcpServer::Connect(const ParameterType& parameters) {
  tcp::socket socket{io_context_};

  std::cout << "waiting for client..." << std::endl;

  const auto address = boost::asio::ip::make_address(parameters.ip);
  tcp::acceptor acceptor_{io_context_, tcp::endpoint{address, parameters.port}};

  acceptor_.accept(socket);

  std::cout << "client connected" << std::endl;
  return std::make_unique<chat::server::BoostTcpConnection>(std::move(socket));
}

}