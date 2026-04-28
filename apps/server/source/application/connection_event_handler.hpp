#pragma once

#include "chat_tcp_system.hpp"

namespace ebroschin::chatcore::server {

class UserServerSystem;

class ConnectionEventHandler final : public ChatServerTcpSystem::ConnectionEventHandler {
public:
  explicit ConnectionEventHandler(UserServerSystem& user_system) noexcept;

  void OnConnected(network::ConnectionId) override;
  void OnConnectionFailed(const network::modules::BoostTcpAcceptorParameters&) override;
  void OnDisconnected(network::ConnectionId connection_id) override;

private:
  UserServerSystem& user_system_;
};

}