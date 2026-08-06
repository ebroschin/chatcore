#pragma once

#include "../application/client_rpc_system.hpp"
#include "../model/model_system.hpp"
#include "../application/chat_client_application.hpp"
#include "session_store.hpp"

#include <ebroschin/chat/api.hpp>
#include <ebroschin/core/system.hpp>

#include <optional>

namespace ebroschin::chatcore::client {

class SessionSystem final : public core::System {
public:
  explicit SessionSystem(const core::SystemContext& ctx, ChatClientApplication& app) noexcept;

  void Initialize() override;

  void Connect(std::string address, std::string port) const;
  void Login(std::string name, std::string password);
  void Logout();
  void Send(std::string message) const;
  void AddUser(std::string name, std::string password);
  void JoinChannel(api::PersistenceId id);
  void CreateChannel(std::string name);
  void GetChannels();
  void Quit() const;

  void ResetUser();

private:
  bool ValidateSession() const;

  void LoadLatestChatLog(api::PersistenceId channel_id);
  void HandleErrorEvent(const api::ErrorMessage& message) const;
  void HandlePrintEvent(const api::PrintMessage& message) const;
  void HandleUserLogoutEvent(const api::UserLogoutEventMessage& message);
  void HandleUserLoginEvent(const api::UserLoginEventMessage& message);
  void HandleChannelCreateEvent(const api::ChannelCreateEventMessage& message);
  void HandleChannelJoinEvent(const api::ChannelJoinEventMessage& message);
  void HandleChannelLeaveEvent(const api::ChannelLeaveEventMessage& message);
  void HandleReceiveChatEvent(const api::ReceiveChatMessage& message);

  void ProcessChatMessage(const api::User& user, const std::string& content) const;

  ChatClientApplication& app_;
  ClientTcpSystem& tcp_system_;
  ModelSystem& model_system_;
  ClientRpcSystem& rpc_system_;

  std::optional<api::User> user_{std::nullopt};
  SessionStore store_{rpc_system_, app_};

  friend class SessionStore;
};

}