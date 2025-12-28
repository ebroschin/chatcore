#pragma once

#include "boost/asio.hpp"
#include "message_handler_registry.h"
#include <claw/core/application.h>
#include <claw/core/system.h>
#include <concepts>
#include <iostream>

#include "tcp_connection.h"
#include "tcp_connector.h"

using boost::asio::ip::tcp;

namespace claw::communication {

class TcpSystem : public core::System {
public:
  explicit TcpSystem(const core::SystemContext& ctx,
    core::Application& app)
    : System(ctx), app_{app}
  { }

  template<typename TMessageHandler, typename... TArgs>
  void RegisterMessageHandler(const std::string& key, TArgs&&... args) {
    message_handler_registry_.Register<TMessageHandler>(key, std::forward<TArgs>(args)...);
  }
  
  void Update() override;
  void SendMessage(const std::string& message);

protected:
  virtual void Connect() = 0;
  void HandleMessage(const std::string& message);

  core::Application& app_;
  std::unique_ptr<TcpConnection> connection_;
  MessageHandlerRegistry message_handler_registry_;
};

template<typename TTcpConnector, typename TTcpConnection>
requires std::derived_from<TTcpConnector, TcpConnector<TTcpConnection>>
  && std::derived_from<TTcpConnection, TcpConnection>
class TcpServerSystem final : public TcpSystem {
public:
  explicit TcpServerSystem(const core::SystemContext& ctx,
    core::Application& app,
    const std::string& address,
    unsigned short port)
    : TcpSystem(ctx, app),
    connector_{std::make_unique<TTcpConnector>(address, port)}
  {}

  void Connect() override {
    if (connection_ != nullptr && connection_->IsOpen()) return;
    std::cout << "waiting for client..." << std::endl;
    connection_ = connector_->Connect();
    std::cout << "client connected!" << std::endl;
  }

private:
  std::unique_ptr<TTcpConnector> connector_;
};

template<typename TTcpConnector, typename TTcpConnection>
requires std::derived_from<TTcpConnector, TcpConnector<TTcpConnection>>
  && std::derived_from<TTcpConnection, TcpConnection>
class TcpClientSystem final : public TcpSystem {
public:
  explicit TcpClientSystem(const core::SystemContext& ctx,
    core::Application& app,
    const std::string& address,
    unsigned short port)
    : TcpSystem(ctx, app),
    connector_{std::make_unique<TTcpConnector>(address, port)}
  {}

  void Connect() override {
    if (connection_ != nullptr && connection_->IsOpen()) return;
    std::cout << "connecting to server..." << std::endl;
    connection_ = connector_->Connect();
    std::cout << "connected!" << std::endl;
  }

private:
  std::unique_ptr<TTcpConnector> connector_;
};

}