#pragma once
#include <memory>

namespace claw::communication {

class TcpConnection;

template<typename TTcpConnection>
requires std::derived_from<TTcpConnection, TcpConnection>
class TcpServer {
public:
  typedef TTcpConnection ConnectionType;

  virtual std::unique_ptr<TTcpConnection> AcceptClientConnection() = 0;
};

}
