#pragma once

#include "boost/asio.hpp"
#include "sync_boost_tcp_connection.h"

#include <memory>

namespace claw::network::modules {

struct SyncBoostTcpAcceptorParameters {
  std::string ip;
  unsigned short port;
};

class SyncBoostTcpAcceptor {
public:
  using ConnectionType = SyncBoostTcpConnection;
  using ParameterType = SyncBoostTcpAcceptorParameters;
  using CallbackType = std::function<void(std::shared_ptr<ConnectionType>)>;

  ~SyncBoostTcpAcceptor();

  void Initialize() {}
  void Connect(const SyncBoostTcpAcceptorParameters& parameters, CallbackType callback);

private:
  void Accept(const ParameterType& parameters, CallbackType callback);

  bool running_{true};
  boost::asio::io_context io_context_{};
  std::thread io_thread_{};
  utility::TaskThread task_thread_; //TODO use io_thread_ instead
};

}