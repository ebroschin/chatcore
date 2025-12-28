#include "boost_tcp_server.h"

#include <ranges>

namespace claw::communication {

std::unique_ptr<chat::server::BoostTcpConnection> BoostTcpServer::Connect() {
  tcp::socket socket{io_context_};
  acceptor_.accept(socket);
  return std::make_unique<chat::server::BoostTcpConnection>(std::move(socket));
}

}