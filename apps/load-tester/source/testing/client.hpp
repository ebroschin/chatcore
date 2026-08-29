#pragma once

#include "../application/client_rpc_system.hpp"
#include "../application/client_tcp_system.hpp"

#include <ebroschin/core/system_context.hpp>
#include <ebroschin/network/commons.hpp>
#include <ebroschin/logging/log.hpp>

#include <chrono>
#include <string>

namespace ebroschin::chatcore::tester {

class LoadTesterApplication;

class Client {
public:
  using PrepareCallback = std::function<void(network::ConnectionId)>;

  explicit Client(LoadTesterApplication& app, core::SystemContext& ctx, std::string name, core::Executor& executor) noexcept;
  virtual ~Client() = default;

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;
  Client(Client&&) = delete;
  Client& operator=(Client&&) = delete;

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

    rpc_call.SetTimeoutDuration(10s);
    rpc_call.OnTimeout([this, message] {
      logging::Log::Error() << "[Client][" << name_ << "] " << message;
      Quit();
    });
  }

  LoadTesterApplication& app_;
  core::SystemContext& ctx_;
  core::Executor& executor_;
  ClientTcpSystem& tcp_system_;
  ClientRpcSystem& rpc_system_;
  std::string name_;

  std::optional<network::ConnectionId> connection_id_{};
  std::optional<api::User> user_{};

private:
  static constexpr std::string_view TestPassword = "123";

  void OnConnected(network::ConnectionId connection_id);
  void OnUserEnsured();
};

}