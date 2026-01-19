#include "boost_tcp_server.h"

#include <iostream>
#include <ranges>

namespace claw::communication {

BoostTcpServer::~BoostTcpServer() {
  if (!worker_.joinable()) return;
  worker_.join();
}

void BoostTcpServer::Connect(const ParameterType& parameters, CallbackType callback) {
  worker_ = std::thread{[this, parameters, callback = std::move(callback)]() {
    Accept(parameters, std::move(callback));
  }};
}

void BoostTcpServer::Accept(const ParameterType& parameters, CallbackType callback) {
  while (true) {
    tcp::socket socket{io_context_};

    std::cout << "waiting for client..." << std::endl;

    const auto address = boost::asio::ip::make_address(parameters.ip);
    tcp::acceptor acceptor_{io_context_, tcp::endpoint{address, parameters.port}};
    acceptor_.accept(socket);

    std::cout << "client connected" << std::endl;

    callback(std::make_unique<chat::server::BoostTcpConnection>(std::move(socket), task_thread_));
  }
}

}