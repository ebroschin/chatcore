#pragma once

#include "../application/client_tcp_system.h"

namespace claw::chat::client {

class SessionSystem;

class ConnectionEventHandler final : public ClientTcpSystem::ConnectionEventHandler {
 public:
  explicit ConnectionEventHandler(SessionSystem& session_system) noexcept;

  void OnConnected(network::ConnectionId connection_id) override;
  void OnConnectionFailed(const network::modules::BoostTcpResolverParameters& parameters) override;
  void OnDisconnected(network::ConnectionId connection_id) override;

private:
  SessionSystem& session_system_;
};

}