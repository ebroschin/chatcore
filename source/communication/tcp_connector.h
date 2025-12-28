#pragma once
#include <memory>

namespace claw::communication {

class TcpConnection;

template<typename TTcpConnection>
requires std::derived_from<TTcpConnection, TcpConnection>
class TcpConnector {
public:
  typedef TTcpConnection ConnectionType;
  virtual std::unique_ptr<TTcpConnection> Connect() = 0;
};

}
