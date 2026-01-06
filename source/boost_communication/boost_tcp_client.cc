#include "boost_tcp_client.h"

namespace claw::communication {

std::unique_ptr<BoostTcpClient::ConnectionType> BoostTcpClient::Connect(const ParameterType& parameters) {
  boost::system::error_code error;
  const auto endpoints = resolver_.resolve(parameters.ip, parameters.port);

  tcp::socket socket{io_context_};
  boost::asio::connect(socket, endpoints, error);
  if (error) return nullptr;

  return std::make_unique<chat::server::BoostTcpConnection>(std::move(socket));
}


}