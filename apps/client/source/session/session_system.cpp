#include "session_system.hpp"

#include "../application/chat_client_application.hpp"
#include "../model/model_system.hpp"
#include "../ui/ui_system.hpp"

#include <ebroschin/core/system_context.hpp>
#include <ebroschin/logging/log.hpp>

namespace ebroschin::chatcore::client {

SessionSystem::SessionSystem(const core::SystemContext& ctx, ChatClientApplication& app) noexcept:
  System{ctx},
  app_{app},
  tcp_system_{ctx.Require<ClientTcpSystem>()},
  model_system_{ctx.Require<ModelSystem>()},
  rpc_system_{ctx.Require<ClientRpcSystem>()}
{ }

void SessionSystem::Initialize() {
  app_.RegisterEventMessageHandler(this, &SessionSystem::HandleErrorEvent);
  app_.RegisterEventMessageHandler(this, &SessionSystem::HandlePrintEvent);
  app_.RegisterEventMessageHandler(this, &SessionSystem::HandleUserLoginEvent);
  app_.RegisterEventMessageHandler(this, &SessionSystem::HandleUserLogoutEvent);
  app_.RegisterEventMessageHandler(this, &SessionSystem::HandleReceiveChatEvent);
  app_.RegisterEventMessageHandler(this, &SessionSystem::HandleChannelCreateEvent);
  app_.RegisterEventMessageHandler(this, &SessionSystem::HandleChannelJoinEvent);
  app_.RegisterEventMessageHandler(this, &SessionSystem::HandleChannelLeaveEvent);
}

bool SessionSystem::ValidateSession() const {
  return connection_id_ && user_;
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
    store_.CacheUser(response.user);
    user_.emplace(response.user);
    model_system_.AddLine("[Client] Logged in as " + user_->name);
  });

  app_.RegisterDefaultErrorHandler(authenticate_user_rpc);
  app_.RegisterDefaultTimeoutHandler(authenticate_user_rpc, "Timed out authenticating user");
  authenticate_user_rpc.Call();
}

void SessionSystem::Logout() {
  if (!ValidateSession()) return;

  auto user_copy = *user_;
  user_.reset();
  model_system_.SetChannelName(std::nullopt);

  auto logout_rpc = rpc_system_.Prepare<api::LogoutRequestMessage>(*connection_id_);
  logout_rpc.OnSuccess([this, user_copy]
  (const api::LogoutResponseMessage&)
  {
    model_system_.AddLine("[Client] Logged out as " + user_copy.name);
  });

  app_.RegisterDefaultErrorHandler(logout_rpc);
  app_.RegisterDefaultTimeoutHandler(logout_rpc, "Timed out logging out user");
  logout_rpc.Call();
}

void SessionSystem::Send(std::string message) const {
  if (!connection_id_) return;
  tcp_system_.Send<api::WriteChatMessage>(*connection_id_, {std::move(message)});
}

void SessionSystem::AddUser(std::string name, std::string password) {
  if (!connection_id_) return;

  auto create_user_rpc = rpc_system_.Prepare<api::CreateUserRequestMessage>(*connection_id_, std::move(name), std::move(password));
  create_user_rpc.OnSuccess([this]
  (const api::CreateUserResponseMessage& response)
  {
    store_.CacheUser(response.user);
    model_system_.AddLine("[Client] User successfully created: " + response.user.name);
  });

  app_.RegisterDefaultErrorHandler(create_user_rpc);
  app_.RegisterDefaultTimeoutHandler(create_user_rpc, "Timed out creating user");
  create_user_rpc.Call();
}

void SessionSystem::GetChannels() {
  if (!connection_id_) return;

  auto get_channels_rpc = rpc_system_.Prepare<api::GetChatChannelsRequestMessage>(*connection_id_);
  get_channels_rpc.OnSuccess([this]
  (const api::GetChatChannelsResponseMessage& response)
  {
    std::stringstream stream{};
    stream << "Available channels: \n";
    for (const auto& channel : response.channels) {
      store_.CacheChannel(channel);
      stream << channel.id << ": " << channel.name << "\n";
    }

    model_system_.AddLine("[Client] " + stream.str());
  });

  app_.RegisterDefaultErrorHandler(get_channels_rpc);
  app_.RegisterDefaultTimeoutHandler(get_channels_rpc, "Timed out requesting channels");
  get_channels_rpc.Call();
}

void SessionSystem::CreateChannel(std::string name) {
  if (!ValidateSession()) return;

  auto create_channel_rpc = rpc_system_.Prepare<api::CreateChannelRequestMessage>(*connection_id_, std::move(name));
  create_channel_rpc.OnSuccess([this]
  (const api::CreateChannelResponseMessage& response)
  {
    store_.CacheChannel(response.channel);
    model_system_.AddLine("[Client] Created channel:\n" + std::to_string(response.channel.id) + ": " + response.channel.name);
  });

  app_.RegisterDefaultErrorHandler(create_channel_rpc);
  app_.RegisterDefaultTimeoutHandler(create_channel_rpc, "Timed out creating channel");
  create_channel_rpc.Call();
}

void SessionSystem::JoinChannel(api::PersistenceId id) {
  if (!ValidateSession()) return;

  auto join_channel_rpc = rpc_system_.Prepare<api::JoinChatChannelRequestMessage>(*connection_id_, id);
  join_channel_rpc.OnSuccess([this]
  (const api::JoinChatChannelResponseMessage& response)
  {
    if (!ValidateSession()) return;

    store_.LoadChannel(*connection_id_, response.channel_id, [this]
    (std::optional<std::reference_wrapper<const api::ChatChannel>> channel)
    {
      if (!channel) return;

      model_system_.AddLine("[Client] Joined channel: " + channel->get().name);
      model_system_.SetChannelName(channel->get().name);
      LoadLatestChatLog(channel->get().id);
    });
  });

  app_.RegisterDefaultErrorHandler(join_channel_rpc);
  app_.RegisterDefaultTimeoutHandler(join_channel_rpc, "Timed out joining channel");
  join_channel_rpc.Call();
}

void SessionSystem::ProcessChatMessage(const api::User& user, const std::string& content) const {
  if (user.id == user_->id) {
    model_system_.AddLine("[You] " + content);
    return;
  }

  model_system_.AddLine("[User::" + user.name + "] " + content);
}

void SessionSystem::LoadLatestChatLog(api::PersistenceId channel_id) {
  if (!ValidateSession()) return;

  auto get_chats_rpc = rpc_system_.Prepare<api::GetChatsRequestMessage>(*connection_id_, channel_id, 20u);
  get_chats_rpc.OnSuccess([this]
  (const api::GetChatsResponseMessage& response)
  {
    if (!ValidateSession()) return;

    std::vector<api::PersistenceId> user_ids{};
    user_ids.reserve(response.messages.size());

    for (const auto& chat : response.messages) {
      user_ids.emplace_back(chat.user_id);
    }

    std::ranges::sort(user_ids);
    user_ids.erase(std::ranges::unique(user_ids).begin(), user_ids.end());

    store_.LoadUsers(*connection_id_, user_ids, [this, messages = response.messages]
    (SessionStore::UsersView view)
    {
      for (const auto& chat_message : messages) {
        const auto it = view.find(chat_message.user_id);
        if (it == view.end()) continue;

        ProcessChatMessage(it->second, chat_message.content);
      }
    });
  });

  app_.RegisterDefaultErrorHandler(get_chats_rpc);
  app_.RegisterDefaultTimeoutHandler(get_chats_rpc, "Timed out requesting chatlog");
  get_chats_rpc.Call();
}

void SessionSystem::Quit() const {
  app_.Quit();
}

void SessionSystem::HandleErrorEvent(const api::ErrorMessage& message) const {
  if (!connection_id_) return;
  logging::Log::Error() << message.value;
}

void SessionSystem::HandlePrintEvent(const api::PrintMessage& message) const {
  if (!connection_id_) return;
  model_system_.AddLine("[Client] Server says: " + message.value);
}

void SessionSystem::HandleUserLogoutEvent(const api::UserLogoutEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  store_.LoadUser(*connection_id_, message.user_id, [this]
  (std::optional<std::reference_wrapper<const api::User>> user)
  {
    if (!user) return;
    model_system_.AddLine("[Client] " + user->get().name + " logged out.");
  });
}

void SessionSystem::HandleUserLoginEvent(const api::UserLoginEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  store_.LoadUser(*connection_id_, message.user_id, [this]
  (std::optional<std::reference_wrapper<const api::User>> user)
  {
    if (!user) return;
    model_system_.AddLine("[Client] " + user->get().name + " logged in.");
  });
}

void SessionSystem::HandleReceiveChatEvent(const api::ReceiveChatMessage& message) {
  if (!ValidateSession()) return;

  store_.LoadUser(*connection_id_, message.user_id, [this, content = message.content]
  (std::optional<std::reference_wrapper<const api::User>> user)
  {
    if (!user) return;
    ProcessChatMessage(user->get(), content);
  });
}

void SessionSystem::HandleChannelCreateEvent(const api::ChannelCreateEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  store_.CacheChannel(message.channel);
  store_.LoadUser(*connection_id_, message.user_id, [this, channel = message.channel]
  (std::optional<std::reference_wrapper<const api::User>> user)
  {
    if (!user) return;
    model_system_.AddLine("[Client] " + user->get().name + " created channel: " + channel.name);
  });
}

void SessionSystem::HandleChannelJoinEvent(const api::ChannelJoinEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  store_.LoadChannel(*connection_id_, message.channel_id, [this, user_id = message.user_id]
  (std::optional<std::reference_wrapper<const api::ChatChannel>> channel)
  {
    if (!ValidateSession()) return;
    if (!channel) return;

    store_.LoadUser(*connection_id_, user_id, [this, channel_name = channel->get().name]
    (std::optional<std::reference_wrapper<const api::User>> user)
    {
      if (!user) return;
      model_system_.AddLine("[Client] " + user->get().name + " joined channel: " + channel_name);
    });
  });
}

void SessionSystem::HandleChannelLeaveEvent(const api::ChannelLeaveEventMessage& message) {
  if (!ValidateSession()) return;
  if (user_->id == message.user_id) return;

  store_.LoadChannel(*connection_id_, message.channel_id, [this, user_id = message.user_id]
  (std::optional<std::reference_wrapper<const api::ChatChannel>> channel)
  {
    if (!ValidateSession()) return;
    if (!channel) return;

    store_.LoadUser(*connection_id_, user_id, [this, channel_name = channel->get().name]
    (std::optional<std::reference_wrapper<const api::User>> user)
    {
      if (!user) return;
      model_system_.AddLine("[Client] " + user->get().name + " left channel: " + channel_name);
    });
  });
}

}