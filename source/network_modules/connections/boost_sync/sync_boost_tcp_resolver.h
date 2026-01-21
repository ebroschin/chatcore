#pragma once

#include "boost/asio.hpp"
#include "sync_boost_tcp_connection.h"

using boost::asio::ip::tcp;

namespace claw::network::modules {

struct SyncBoostTcpResolverParameters {
  std::string ip;
  std::string port;
};

class SyncBoostTcpResolver {
public:
  using ConnectionType = SyncBoostTcpConnection;
  using ParameterType = SyncBoostTcpResolverParameters;
  using CallbackType = std::function<void(std::shared_ptr<ConnectionType>)>;

  void Connect(const ParameterType& parameters, CallbackType callback);

private:
  boost::asio::io_context io_context_{};
  boost::asio::ip::tcp::resolver resolver_{io_context_};
  utility::TaskThread task_thread_;
};

}