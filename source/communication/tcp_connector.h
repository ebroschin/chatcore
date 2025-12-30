#pragma once
#include <memory>

namespace claw::communication {

class TcpConnection;

template<typename TTcpConnection, typename TConnectionParameters>
requires std::derived_from<TTcpConnection, TcpConnection>
class TcpConnector {
public:
  using ConnectionType = TTcpConnection;
  using ParameterType = TConnectionParameters;

  virtual std::unique_ptr<TTcpConnection> Connect(const TConnectionParameters& parameters) = 0;
};

}
