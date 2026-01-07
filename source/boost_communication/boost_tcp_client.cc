#include "boost_tcp_client.h"

namespace claw::communication {

void BoostTcpClient::Connect(const ParameterType& parameters, CallbackType callback) {
  boost::system::error_code error;
  const auto endpoints = resolver_.resolve(parameters.ip, parameters.port);

  tcp::socket socket{io_context_};
  boost::asio::connect(socket, endpoints, error);
  if (error) return;

  callback(std::make_unique<chat::server::BoostTcpConnection>(std::move(socket)));
}


}