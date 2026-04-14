#include <claw/network-modules/connectors/boost_async/boost_tcp_acceptor.h>
#include <iostream>

namespace claw::network::modules {

BoostTcpAcceptor::~BoostTcpAcceptor() {
  if (acceptor_ && acceptor_->is_open()) {
    system::error_code error;
    acceptor_->cancel(error);
    acceptor_->close(error);
  }

  work_guard_.reset();
  io_context_.stop();
  io_thread_ = {};
}

void BoostTcpAcceptor::Start() {
  io_thread_ = std::jthread{[this] { io_context_.run(); }};
}

void BoostTcpAcceptor::Connect(BoostTcpAcceptorParameters parameters, ConnectionEventHandler* connection_event_handler) {
  if (acceptor_ != nullptr) return;

  const auto address = asio::ip::make_address(parameters.ip);
  acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(io_context_, asio::ip::tcp::endpoint{address, parameters.port});
  connection_event_handler_ = connection_event_handler;
  StartAccept();
}

void BoostTcpAcceptor::StartAccept() {
  std::cout << "waiting for client" << std::endl;
  acceptor_->async_accept([this](const system::error_code& error, asio::ip::tcp::socket socket) {
    if (error == asio::error::operation_aborted) return;

    if (!error) {
      std::cout << "client connected" << std::endl;
      OnConnectionCreated(std::make_shared<Connection>(std::move(socket)), connection_event_handler_);
    }

    StartAccept();
  });
}

}