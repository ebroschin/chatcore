#include "session_system.h"
#include "../application/application_system.h"
#include "../model/model_system.h"
#include "../ui/ui_system.h"
#include "ebroschin/core/system_context.h"

#include "ebroschin/logging/log.hpp"

namespace ebroschin::chatcore::client {

SessionSystem::SessionSystem(const core::SystemContext& ctx) noexcept:
  System(ctx),
  tcp_system_(ctx.Require<ClientTcpSystem>()),
  app_system_(ctx.Require<ApplicationSystem>()),
  model_system_(ctx.Require<ModelSystem>()),
  rpc_system_(ctx.Require<ClientRpcSystem>())
{ }

void SessionSystem::Initialize() {
  RegisterEventMessageHandler(&SessionSystem::HandleErrorEvent);
  RegisterEventMessageHandler(&SessionSystem::HandlePrintEvent);
  RegisterEventMessageHandler(&SessionSystem::HandleUserLoginEvent);
  RegisterEventMessageHandler(&SessionSystem::HandleUserLogoutEvent);
  RegisterEventMessageHandler(&SessionSystem::HandleReceiveChatEvent);
  RegisterEventMessageHandler(&SessionSystem::HandleChannelCreateEvent);
  RegisterEventMessageHandler(&SessionSystem::HandleChannelJoinEvent);
  RegisterEventMessageHandler(&SessionSystem::HandleChannelLeaveEvent);
}

bool SessionSystem::ValidateSession() const {
  if (!connection_id_) return false;
  if (!user_) return false;
  return true;
}

void SessionSystem::Connect(std::string address, std::string port) {
  tcp_system_.Connect({std::move(address), std::move(port)}, &connection_event_handler_);
}

void SessionSystem::Login(std::string name, std::string password) {
  if (!connection_id_) return;

  auto authenticate_user_rpc = rpc_system_.Prepare<api::AuthenticateUserRequestMessage>(*connection_id_, std::move(name), std::move(password));
  authenticate_user_rpc.OnSuccess([this]
    (const api::AuthenticateUserResponseMessage& response)
  {
    user_.emplace(response.user);
    model_system_.AddLine("[Client] Logged in as " + user_->name);
  });

  RegisterDefaultErrorHandler(authenticate_user_rpc);
  RegisterDefaultTimeoutHandler(authenticate_user_rpc, "Timed out authenticating user");
  authenticate_user_rpc.Call();
}

void SessionSystem::Logout() {
  if (!connection_id_) return;
  if (!user_) return;

  auto user_copy = *user_;
  user_.reset();
  model_system_.SetChannelName(std::nullopt);

  auto logout_rpc = rpc_system_.Prepare<api::LogoutRequestMessage>(*connection_id_);
  logout_rpc.OnSuccess([this, user_copy]
    (const api::LogoutResponseMessage&)
  {
    model_system_.AddLine("[Client] Logged out as " + user_copy.name);
  });

  RegisterDefaultErrorHandler(logout_rpc);
  RegisterDefaultTimeoutHandler(logout_rpc, "Timed out logging out user");
  logout_rpc.Call();
}

void SessionSystem::Send(std::string message) const {
  if (!connection_id_) return;
  tcp_system_.Send<api::WriteChatMessage>(*connection_id_, {std::move(message)});
}

void SessionSystem::AddUser(std::string name, std::string password) {
  if (!connection_id_) return;

  auto create_user_rpc = rpc_system_.Prepare<api::CreateUserRequestMessage>(*connection_id_, std::move(name), std::move(password));
  create_user_rpc.OnSuccess([this](const api::CreateUserResponseMessage& response) {
    users_cache_.try_emplace(response.user.id, response.user);
    model_system_.AddLine("[Client] User successfully created: " + response.user.name);
  });

  RegisterDefaultErrorHandler(create_user_rpc);
  RegisterDefaultTimeoutHandler(create_user_rpc, "Timed out creating user");
  create_user_rpc.Call();
}

void SessionSystem::GetChannels() const {
  if (!connection_id_) return;

  auto get_channels_rpc = rpc_system_.Prepare<api::GetChatChannelsRequestMessage>(*connection_id_);
  get_channels_rpc.OnSuccess([this](const api::GetChatChannelsResponseMessage& response) {
    std::stringstream stream;
    stream << "Available channels: \n";
    for (const auto& channel : response.channels) {
      stream << channel.id << ": " << channel.name;
    }

    model_system_.AddLine("[Client] " + stream.str());
  });

  RegisterDefaultErrorHandler(get_channels_rpc);
  RegisterDefaultTimeoutHandler(get_channels_rpc, "Timed out requesting channels");
  get_channels_rpc.Call();
}

void SessionSystem::CreateChannel(std::string name) const {
  if (!connection_id_) return;

  auto create_channel_rpc = rpc_system_.Prepare<api::CreateChannelRequestMessage>(*connection_id_, std::move(name));
  create_channel_rpc.OnSuccess([this](const api::CreateChannelResponseMessage& response) {
    model_system_.AddLine("[Client] Created channel:\n" + std::to_string(response.channel.id) + ": " + response.channel.name);
  });

  RegisterDefaultErrorHandler(create_channel_rpc);
  RegisterDefaultTimeoutHandler(create_channel_rpc, "Timed out creating channel");
  create_channel_rpc.Call();
}

void SessionSystem::JoinChannel(api::PersistenceId id) {
  if (!connection_id_) return;

  auto join_channel_rpc = rpc_system_.Prepare<api::JoinChatChannelRequestMessage>(*connection_id_, id);
  join_channel_rpc.OnSuccess([this](const api::JoinChatChannelResponseMessage& response) {
    LoadChannel(response.channel_id, [this](std::optional<std::reference_wrapper<const api::ChatChannel>> channel) {
      if (!channel) return;

      model_system_.AddLine("[Client] Joined channel: " + channel->get().name);
      model_system_.SetChannelName(channel->get().name);
      LoadLatestChatLog(channel->get().id);
    });
  });

  RegisterDefaultErrorHandler(join_channel_rpc);
  RegisterDefaultTimeoutHandler(join_channel_rpc, "Timed out joining channel");
  join_channel_rpc.Call();
}

void SessionSystem::LoadLatestChatLog(api::PersistenceId channel_id) {
  if (!connection_id_) return;

  auto get_chats_rpc = rpc_system_.Prepare<api::GetChatsRequestMessage>(*connection_id_, channel_id, 20u);
  get_chats_rpc.OnSuccess([this](const api::GetChatsResponseMessage& response)
  {
    std::vector<api::PersistenceId> user_ids;
    user_ids.reserve(response.messages.size());

    for (const auto& chat : response.messages) {
      user_ids.emplace_back(chat.user_id);
    }

    std::ranges::sort(user_ids);
    user_ids.erase(std::ranges::unique(user_ids).begin(), user_ids.end());

    LoadUsers(user_ids, [this, messages = response.messages]() {
      for (const auto& chat : messages) {
        const auto it = users_cache_.find(chat.user_id);
        if (it == users_cache_.end()) continue;

        model_system_.AddLine("[User::" + it->second.name + "] " + chat.content);
      }
    });
  });

  RegisterDefaultErrorHandler(get_chats_rpc);
  RegisterDefaultTimeoutHandler(get_chats_rpc, "Timed out requesting chatlog");
  get_chats_rpc.Call();
}

void SessionSystem::LoadUsers(std::span<const api::PersistenceId> user_ids, std::function<void()> callback) {
  std::vector<api::PersistenceId> uncached_ids;
  for (const auto& user_id : user_ids) {
    const auto it = users_cache_.find(user_id);
    if (it != users_cache_.end()) continue;
    uncached_ids.emplace_back(user_id);
  }

  if (uncached_ids.empty()) {
    if (!callback) return;
    callback();
    return;
  }

  auto get_user_rpc = rpc_system_.Prepare<api::GetUsersRequestMessage>(*connection_id_, uncached_ids);
  get_user_rpc.OnSuccess([this, callback = std::move(callback)]
    (const api::GetUsersResponseMessage& response) mutable
  {
    for (const auto& user : response.users) {
      const auto id = user.id;
      users_cache_.try_emplace(id, user);
    }

    if (!callback) return;
    callback();
  });

  RegisterDefaultErrorHandler(get_user_rpc);
  RegisterDefaultTimeoutHandler(get_user_rpc, "Timed out requesting user");
  get_user_rpc.Call();
}

void SessionSystem::LoadUser(api::PersistenceId user_id, std::function<void(std::optional<std::reference_wrapper<const api::User>> user)> callback) {
  LoadUsers(std::vector{user_id}, [this, user_id, callback = std::move(callback)]() {
    if (!callback) return;

    const auto it = users_cache_.find(user_id);
    if (it == users_cache_.end()) {
      callback(std::nullopt);
      return;
    }

    callback(it->second);
  });
}

void SessionSystem::LoadChannel(api::PersistenceId channel_id, std::function<void(std::optional<std::reference_wrapper<const api::ChatChannel>> channel)> callback) {
  const auto it = channels_cache_.find(channel_id);
  if (it != channels_cache_.end()) {
    if (!callback) return;
    callback(it->second);
    return;
  }

  //prewarm all channels on demand only
  auto get_channels_rpc = rpc_system_.Prepare<api::GetChatChannelsRequestMessage>(*connection_id_);
  get_channels_rpc.OnSuccess([this, channel_id, callback = std::move(callback)]
    (const api::GetChatChannelsResponseMessage& response)
  {
    std::optional<std::reference_wrapper<const api::ChatChannel>> result;
    for (const auto& channel : response.channels) {
      channels_cache_.try_emplace(channel.id, channel);

      if (channel_id != channel.id) continue;
      result.emplace(channel);
    }

    if (!callback) return;
    callback(result);
  });

  RegisterDefaultErrorHandler(get_channels_rpc);
  RegisterDefaultTimeoutHandler(get_channels_rpc, "Timed out requesting channels");
  get_channels_rpc.Call();
}

void SessionSystem::OnConnected(network::ConnectionId connection_id) {
  connection_id_.emplace(connection_id);
  logging::Log::Info() << "Successfully connected to chat server.";
}

void SessionSystem::OnConnectionFailed(const network::modules::BoostTcpResolverParameters& parameters) const {
  logging::Log::Error() << "Connection to chat server failed " << parameters.ip + ":" + parameters.port;
}

void SessionSystem::OnDisconnected(network::ConnectionId) {
  connection_id_.reset();
  user_.reset();
  logging::Log::Info() << "Lost connection to chat server.";
  model_system_.SetChannelName(std::nullopt);
}

void SessionSystem::HandleErrorEvent(const api::ErrorMessage& message) {
  if (!connection_id_) return;
  logging::Log::Error() << message.value;
}

void SessionSystem::HandlePrintEvent(const api::PrintMessage& message) {
  if (!connection_id_) return;
  model_system_.AddLine("[Client] Server says: " + message.value);
}

void SessionSystem::HandleUserLogoutEvent(const api::UserLogoutEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  LoadUser(message.user_id, [this]
  (std::optional<std::reference_wrapper<const api::User>> user)
  {
    if (!user) return;
    model_system_.AddLine("[Client] " + user->get().name + " logged out.");
  });
}

void SessionSystem::HandleUserLoginEvent(const api::UserLoginEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  LoadUser(message.user_id, [this]
  (std::optional<std::reference_wrapper<const api::User>> user)
  {
    if (!user) return;
    model_system_.AddLine("[Client] " + user->get().name + " logged in.");
  });
}

void SessionSystem::HandleReceiveChatEvent(const api::ReceiveChatMessage& message) {
  if (!ValidateSession()) return;

  LoadUser(message.user_id, [this, content = message.content]
  (std::optional<std::reference_wrapper<const api::User>> user)
  {
    if (!user) return;
    model_system_.AddLine("[User::" + user->get().name + "] " + content);
  });
}

void SessionSystem::HandleChannelCreateEvent(const api::ChannelCreateEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  channels_cache_.try_emplace(message.channel.id, message.channel);

  LoadUser(message.user_id, [this, message]
  (std::optional<std::reference_wrapper<const api::User>> user)
  {
    if (!user) return;
    model_system_.AddLine("[Client] " + user->get().name + " created channel: " + message.channel.name);
  });
}

void SessionSystem::HandleChannelJoinEvent(const api::ChannelJoinEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  LoadChannel(message.channel_id, [this, message](std::optional<std::reference_wrapper<const api::ChatChannel>> channel) {
    if (!channel) return;

    LoadUser(message.user_id, [this, channel]
    (std::optional<std::reference_wrapper<const api::User>> user)
    {
      if (!user) return;
      model_system_.AddLine("[Client] " + user->get().name + " joined channel: " + channel->get().name);
    });
  });
}

void SessionSystem::HandleChannelLeaveEvent(const api::ChannelLeaveEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  LoadChannel(message.channel_id, [this, message](std::optional<std::reference_wrapper<const api::ChatChannel>> channel) {
    if (!channel) return;

    LoadUser(message.user_id, [this, channel]
    (std::optional<std::reference_wrapper<const api::User>> user)
    {
      if (!user) return;
      model_system_.AddLine("[Client] " + user->get().name + " left channel: " + channel->get().name);
    });
  });
}

}