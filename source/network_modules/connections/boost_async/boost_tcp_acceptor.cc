#include "boost_tcp_acceptor.h"

#include <iostream>

namespace claw::network::modules {

BoostTcpAcceptor::~BoostTcpAcceptor() {
  io_context_.stop();
  if (!io_thread_.joinable()) return;
  io_thread_.join();
}

void BoostTcpAcceptor::Connect(const BoostTcpAcceptorParameters& parameters, CallbackType callback) {
  if (acceptor_ != nullptr) return; //TODO dont allow multiple Connect attempts for acceptors

  const auto address = boost::asio::ip::make_address(parameters.ip);
  acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(io_context_, boost::asio::ip::tcp::endpoint{address, parameters.port});
  callback_ = std::move(callback);
  StartAccept();

  //TODO use a work_guard in case async_accept is not intended to constantly be in the work queue anymore
  // to prevent the io_context from terminating when it runs out of work
  io_thread_ = std::thread{[&]() { io_context_.run(); }};
}

void BoostTcpAcceptor::StartAccept() {
  std::cout << "waiting for client" << std::endl;
  acceptor_->async_accept([this](const boost::system::error_code& error, boost::asio::ip::tcp::socket socket) {
    HandleAccept(error, std::move(socket));
    StartAccept();
  });
}

void BoostTcpAcceptor::HandleAccept(const boost::system::error_code& error, boost::asio::ip::tcp::socket socket) {
  if (error) return; //TODO logging
  if(!callback_) return;

  std::cout << "client connected" << std::endl;
  callback_(std::make_shared<ConnectionType>(std::move(socket)));
}

}