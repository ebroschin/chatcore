#pragma once

#include "../application/application_system.hpp"
#include "../application/client_rpc_system.hpp"
#include "../model/model_system.hpp"
#include "connection_event_handler.hpp"
#include "session_store.hpp"

#include <ebroschin/logging/log.hpp>
#include <ebroschin/chat/api.hpp>
#include <ebroschin/core/system.hpp>

#include <optional>

namespace ebroschin::chatcore::client {

class SessionSystem final : public core::System {
public:
  explicit SessionSystem(const core::SystemContext& ctx) noexcept;

  void Initialize() override;

  void Connect(std::string address, std::string port);
  void Login(std::string name, std::string password);
  void Logout();
  void Send(std::string message) const;
  void AddUser(std::string name, std::string password);
  void JoinChannel(api::PersistenceId id);
  void CreateChannel(std::string name);
  void GetChannels();

private:
  bool ValidateSession() const;

  void LoadLatestChatLog(api::PersistenceId channel_id);
  void HandleErrorEvent(const api::ErrorMessage& message);
  void HandlePrintEvent(const api::PrintMessage& message);
  void HandleUserLogoutEvent(const api::UserLogoutEventMessage& message);
  void HandleUserLoginEvent(const api::UserLoginEventMessage& message);
  void HandleChannelCreateEvent(const api::ChannelCreateEventMessage& message);
  void HandleChannelJoinEvent(const api::ChannelJoinEventMessage& message);
  void HandleChannelLeaveEvent(const api::ChannelLeaveEventMessage& message);
  void HandleReceiveChatEvent(const api::ReceiveChatMessage& message);

  void ProcessChatMessage(const api::User& user, const std::string& content) const;

  template <typename TMessage>
  void RegisterEventMessageHandler(void(SessionSystem::*method)(const TMessage&)) {
    auto& signals = app_system_.GetMessageHandler();
    auto subscription = signals.Subscribe<TMessage>([this, method]
    (network::ConnectionId, const TMessage& message)
    {
      (this->*method)(message);
    });

    subscriptions_.emplace_back(std::move(subscription));
  }

  template <typename RpcCall>
  void RegisterDefaultErrorHandler(RpcCall&& rpcCall) const {
    rpcCall.OnError([](const api::ErrorResponseMessage& response) {
      logging::Log::Error() << response.value;
    });
  }

  template <typename RpcCall>
  void RegisterDefaultTimeoutHandler(RpcCall&& rpcCall, const std::string& message) const {
    using namespace std::chrono_literals;

    rpcCall.SetTimeoutDuration(5s);
    rpcCall.OnTimeout([message] {
      logging::Log::Error() << message;
    });
  }

  ClientTcpSystem& tcp_system_;
  ApplicationSystem& app_system_;
  ModelSystem& model_system_;
  ClientRpcSystem& rpc_system_;

  std::optional<network::ConnectionId> connection_id_{std::nullopt};
  std::optional<api::User> user_{std::nullopt};
  std::vector<utility::SignalSubscription> subscriptions_{};

  SessionStore store_{*this, rpc_system_};
  ConnectionEventHandler connection_event_handler_{*this};

  friend class ConnectionEventHandler;
  friend class SessionStore;
};

}