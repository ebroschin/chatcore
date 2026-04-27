#pragma once

#include <ebroschin/chat/api.hpp>
#include <ebroschin/core/system.hpp>

#include <optional>

#include "../application/application_system.hpp"
#include "../application/client_rpc_system.hpp"
#include "../model/model_system.hpp"
#include "connection_event_handler.hpp"
#include "ebroschin/logging/log.hpp"

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
  void CreateChannel(std::string name) const;
  void GetChannels() const;

private:
  bool ValidateSession() const;

  void LoadLatestChatLog(api::PersistenceId channel_id);
  void LoadUsers(std::span<const api::PersistenceId> user_ids, std::function<void()> callback);
  void LoadUser(api::PersistenceId user_id, std::function<void(std::optional<std::reference_wrapper<const api::User>> user)> callback);
  void LoadChannel(api::PersistenceId channel_id, std::function<void(std::optional<std::reference_wrapper<const api::ChatChannel>> channel)> callback);

  void OnConnected(network::ConnectionId connection_id);
  void OnConnectionFailed(const network::modules::BoostTcpResolverParameters& parameters) const;
  void OnDisconnected(network::ConnectionId connection_id);

  void HandleErrorEvent(const api::ErrorMessage& message);
  void HandlePrintEvent(const api::PrintMessage& message);
  void HandleUserLogoutEvent(const api::UserLogoutEventMessage& message);
  void HandleUserLoginEvent(const api::UserLoginEventMessage& message);
  void HandleChannelCreateEvent(const api::ChannelCreateEventMessage& message);
  void HandleChannelJoinEvent(const api::ChannelJoinEventMessage& message);
  void HandleChannelLeaveEvent(const api::ChannelLeaveEventMessage& message);
  void HandleReceiveChatEvent(const api::ReceiveChatMessage& message);

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
  ConnectionEventHandler connection_event_handler_{*this};

  std::unordered_map<api::PersistenceId, api::User> users_cache_{};
  std::unordered_map<api::PersistenceId, api::ChatChannel> channels_cache_{};

  friend class ConnectionEventHandler;
};

}