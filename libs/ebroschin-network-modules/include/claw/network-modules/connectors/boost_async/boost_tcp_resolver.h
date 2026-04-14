#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <claw/network/tcp/tcp_connector.h>
#include <claw/network-modules/connectors/boost_async/boost_tcp_connection.h>
#include <string>
#include <thread>

using namespace boost;

namespace claw::network::modules {

struct BoostTcpResolverParameters {
  std::string ip;
  std::string port;
};

class BoostTcpResolver final : public tcp::TcpConnector<BoostTcpResolverParameters, BoostTcpConnection> {
public:
  ~BoostTcpResolver() override;

  void Start() override;
  void Connect(BoostTcpResolverParameters parameters, ConnectionEventHandler* connection_event_handler) override;

private:
  void HandleResolve(const asio::ip::tcp::resolver::results_type& results,
    const BoostTcpResolverParameters& parameters,
    ConnectionEventHandler* connection_event_handler);

  asio::io_context io_context_{};
  asio::ip::tcp::resolver resolver_{io_context_};

  asio::executor_work_guard<asio::io_context::executor_type>
  work_guard_{asio::make_work_guard(io_context_)};

  std::jthread io_thread_{};
};

}