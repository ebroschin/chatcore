#include "sync_boost_tcp_resolver.h"

namespace claw::network::modules {

void SyncBoostTcpResolver::Connect(const ParameterType& parameters, CallbackType callback) {
  boost::system::error_code error;
  const auto endpoints = resolver_.resolve(parameters.ip, parameters.port);

  boost::asio::ip::tcp::socket socket{io_context_};
  boost::asio::connect(socket, endpoints, error);
  if (error) return;

  callback(std::make_unique<SyncBoostTcpConnection>(std::move(socket), task_thread_));
}


}