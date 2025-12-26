#pragma once

#include "../communication/tcp_server.h"
#include "boost/asio.hpp"

#include <SQLiteCpp/SQLiteCpp.h>

using boost::asio::ip::tcp;

namespace claw::communication {

class BoostTCPServer final : public TCPServer {
public:
  explicit BoostTCPServer(const std::string& ip, const unsigned short port)
    : acceptor_{io_context_, tcp::endpoint{boost::asio::ip::make_address(ip), port}},
    db_{"example.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE}
  {}

  void Initialize() override;
  void Update() override;
  void Deinitialize() override;

private:
  boost::asio::io_context io_context_{};
  tcp::acceptor acceptor_;
  tcp::socket socket_{io_context_};

  SQLite::Database db_;
};

}