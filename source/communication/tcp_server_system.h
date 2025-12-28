#pragma once

#include "boost/asio.hpp"
#include "message_handler_registry.h"
#include <claw/core/application.h>
#include <claw/core/system.h>
#include <concepts>
#include <iostream>

#include "tcp_server.h"
#include "tcp_connection.h"

using boost::asio::ip::tcp;

namespace claw::communication {

class TcpServerSystemBase : public core::System {
public:
  explicit TcpServerSystemBase(const core::SystemContext& ctx,
    core::Application& app)
    : System(ctx), app_{app}
  { }

  template<typename TMessageHandler, typename... TArgs>
  void RegisterMessageHandler(const std::string& key, TArgs&&... args) {
    message_handler_registry_.Register<TMessageHandler>(key, std::forward<TArgs>(args)...);
  }

  void Initialize() override;
  void Update() override;
  void SendMessage(const std::string& message);

protected:
  virtual void Connect() = 0;
  void HandleMessage(const std::string& message);

  core::Application& app_;
  std::unique_ptr<TcpConnection> connection_;
  MessageHandlerRegistry message_handler_registry_;
};


template<typename TTcpServer, typename TTcpConnection>
requires std::derived_from<TTcpServer, TcpServer<TTcpConnection>>
  && std::derived_from<TTcpConnection, TcpConnection>
class TcpServerSystem final : public TcpServerSystemBase {
public:
  explicit TcpServerSystem(const core::SystemContext& ctx,
    core::Application& app,
    const std::string& address,
    unsigned short port)
    : TcpServerSystemBase(ctx, app),
    server_{std::make_unique<TTcpServer>(address, port)}
  {}

  void Connect() override {
    if (connection_ != nullptr && connection_->IsOpen()) return;
    std::cout << "waiting for client" << std::endl;
    connection_ = server_->AcceptClientConnection();
    std::cout << "client connected!" << std::endl;
  }

private:
  std::unique_ptr<TTcpServer> server_;
};

}