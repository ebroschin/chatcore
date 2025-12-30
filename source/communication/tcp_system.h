#pragma once

#include "boost/asio.hpp"
#include "message_handler_registry.h"
#include <claw/core/system.h>
#include <concepts>
#include "tcp_connection.h"
#include "tcp_connector.h"

using boost::asio::ip::tcp;

namespace claw::communication {

class TcpSystemBase : public core::System {
public:
  explicit TcpSystemBase(const core::SystemContext& ctx)
    : System(ctx)
  { }

  template<typename TMessageHandler, typename... TArgs>
  void RegisterMessageHandler(const std::string& key, TArgs&&... args) {
    message_handler_registry_.Register<TMessageHandler>(key, std::forward<TArgs>(args)...);
  }

  template<typename TMessageHandler, typename... TArgs>
  void RegisterFallbackMessageHandler(TArgs&&... args) {
    message_handler_registry_.RegisterFallback<TMessageHandler>(std::forward<TArgs>(args)...);
  }

  void Update() override;
  void SendMessage(const std::string& message); //TODO connection id
  //TODO void BroadcastMessage(const std::string& message);

protected:
  void HandleMessage(const std::string& message);

  //TODO refactor to multi-connection later, dont impose connection attempts in Update loop
  // abstract the connection management further to be more customizable
  std::unique_ptr<TcpConnection> connection_;
  MessageHandlerRegistry message_handler_registry_;
};

//TODO specifications for client/server are equivalent
// the only real difference is how the connection gets established
// after the handshake, both server and client are peers
// connector (and its connection type) are passed via template parameters

template<typename TTcpConnector>
requires std::derived_from<TTcpConnector,
  TcpConnector<typename TTcpConnector::ConnectionType, typename TTcpConnector::ParameterType>>
  && std::derived_from<typename TTcpConnector::ConnectionType, TcpConnection>
class TcpSystem : public TcpSystemBase {
public:
  using ParameterType = TTcpConnector::ParameterType;

  explicit TcpSystem(const core::SystemContext& ctx)
    : TcpSystemBase(ctx),
    connector_{std::make_unique<TTcpConnector>()}
  {}

  void Connect(const ParameterType& parameters) {
    //TODO support multi connections
    if (connection_ != nullptr && connection_->IsOpen()) return;
    connection_ = connector_->Connect(parameters);
  }

protected:
  std::unique_ptr<TTcpConnector> connector_;
};

}