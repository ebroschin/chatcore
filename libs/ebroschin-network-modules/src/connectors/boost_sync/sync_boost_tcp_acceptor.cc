#include <claw/network-modules/connectors/boost_sync/sync_boost_tcp_acceptor.h>
#include <iostream>
#include <ranges>

namespace claw::network::modules {

SyncBoostTcpAcceptor::~SyncBoostTcpAcceptor() {
  running_ = false;
  if (!io_thread_.joinable()) return;
  io_thread_.join();
}

void SyncBoostTcpAcceptor::Connect(const ParameterType& parameters, CallbackType callback) {
  io_thread_ = std::thread{[this, parameters, callback = std::move(callback)]() {
    Accept(parameters, callback);
  }};
}

void SyncBoostTcpAcceptor::Accept(const ParameterType& parameters, CallbackType callback) {
  while (running_) {
    boost::asio::ip::tcp::socket socket{io_context_};

    std::cout << "waiting for client..." << std::endl;

    const auto address = boost::asio::ip::make_address(parameters.ip);
    boost::asio::ip::tcp::acceptor acceptor_{io_context_, {address, parameters.port}};
    acceptor_.accept(socket);

    std::cout << "client connected" << std::endl;

    callback(std::make_unique<SyncBoostTcpConnection>(std::move(socket), task_thread_));
  }
}

}