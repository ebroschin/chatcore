#pragma once

#include <claw/network/commons.h>

#include <chrono>
#include <iostream>
#include <string>

#include "../application/client_tcp_system.h"
#include "../application/client_rpc_system.h"

namespace claw::chat::tester {

class ApplicationSystem;

class Client : ClientTcpSystem::ConnectionEventHandler {
public:
  using PrepareCallback = std::function<void(network::ConnectionId)>;

  explicit Client(ApplicationSystem& network_system,
    ClientTcpSystem& tcp_system,
    ClientRpcSystem& rpc_system,
    const std::string& name);

  void Prepare();

protected:
  virtual void OnPrepared() = 0;

  [[nodiscard]] bool ValidateSession() const noexcept {
    return connection_id_ && user_;
  }

  template <typename TRpcCall>
  void RegisterDefaultErrorHandler(TRpcCall& rpc_call) const {
    rpc_call.OnError([this](const api::ErrorResponseMessage& response) {
      std::cerr << "[Server::Error][" << name_ << "] " << response.value << std::endl;
    });
  }

  template <typename TRpcCall>
  void RegisterDefaultTimeoutHandler(TRpcCall& rpc_call, const std::string& message) const {
    using namespace std::chrono_literals;

    rpc_call.SetTimeoutDuration(5s);
    rpc_call.OnTimeout([this, message] {
      std::cerr << "[Client][" << name_ << "] " << message << std::endl;
    });
  }

  ApplicationSystem& app_system_;
  ClientTcpSystem& tcp_system_;
  ClientRpcSystem& rpc_system_;
  std::string name_;

  std::optional<network::ConnectionId> connection_id_{};
  std::optional<api::User> user_{};

private:
  void OnConnected(network::ConnectionId connection_id) override;
  void OnConnectionFailed(const network::modules::BoostTcpResolverParameters&) override {}
  void OnDisconnected(network::ConnectionId) override {}

  void OnUserCreated(const api::CreateUserResponseMessage& message);
};

}