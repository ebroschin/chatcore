#pragma once

#include <claw/network/commons.h>

#include <chrono>
#include <iostream>
#include <string>

#include "../application/client_rpc_system.h"
#include "../application/client_tcp_system.h"
#include <ebroschin/logging/log.hpp>

namespace claw::chat::tester {

class ApplicationSystem;

class Client : ClientTcpSystem::ConnectionEventHandler {
public:
  using PrepareCallback = std::function<void(network::ConnectionId)>;

  explicit Client(ApplicationSystem& network_system,
    ClientTcpSystem& tcp_system,
    ClientRpcSystem& rpc_system,
    std::string name);

  void Prepare();

protected:
  virtual void OnPrepared() = 0;

  template <typename TRpcCall>
  void RegisterDefaultErrorHandler(TRpcCall& rpc_call) const {
    rpc_call.OnError([this](const api::ErrorResponseMessage& response) {
      std::stringstream stream;
      stream << "[Server::Error][" << name_ << "] " << response.value;
      ebroschin::logging::Log::Error(stream.str());
    });
  }

  template <typename TRpcCall>
  void RegisterDefaultTimeoutHandler(TRpcCall& rpc_call, const std::string& message) const {
    using namespace std::chrono_literals;

    rpc_call.SetTimeoutDuration(5s);
    rpc_call.OnTimeout([this, message] {
      std::stringstream stream;
      stream << "[Client][" << name_ << "] " << message;
      ebroschin::logging::Log::Error(stream.str());
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