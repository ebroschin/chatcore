#pragma once

#include <ebroschin/network/commons.hpp>

#include <chrono>
#include <string>

#include "../application/client_rpc_system.hpp"
#include "../application/client_tcp_system.hpp"
#include <ebroschin/logging/log.hpp>

namespace ebroschin::chatcore::tester {

class ApplicationSystem;

class Client : ClientTcpSystem::ConnectionEventHandler {
public:
  using PrepareCallback = std::function<void(network::ConnectionId)>;

  explicit Client(ApplicationSystem& app_system,
    ClientTcpSystem& tcp_system,
    ClientRpcSystem& rpc_system,
    std::string name);

  void Prepare();

protected:
  virtual void OnPrepared() = 0;

  void Quit() const;

  template <typename TRpcCall>
  void RegisterDefaultErrorHandler(TRpcCall& rpc_call) const {
    rpc_call.OnError([this](const api::ErrorResponseMessage& response) {
      logging::Log::Error() << "[Server::Error][" << name_ << "] " << response.value;
      Quit();
    });
  }

  template <typename TRpcCall>
  void RegisterDefaultTimeoutHandler(TRpcCall& rpc_call, const std::string& message) const {
    using namespace std::chrono_literals;

    rpc_call.SetTimeoutDuration(30s);
    rpc_call.OnTimeout([this, message] {
      logging::Log::Error() << "[Client][" << name_ << "] " << message;
      Quit();
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