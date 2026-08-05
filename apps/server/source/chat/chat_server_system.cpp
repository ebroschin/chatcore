#include "chat_server_system.hpp"

#include "../application/chat_server_application.hpp"
#include "../application/chat_persistence_system.hpp"
#include "../users/user_server_system.hpp"
#include "adapters/chat_persistence_adapter.hpp"

#include <ebroschin/network/tcp/tcp_system.hpp>
#include <ebroschin/scheduling/scheduling_system.hpp>
#include <ebroschin/core/system_context.hpp>

using namespace std::chrono_literals;

namespace ebroschin::chatcore::server {

ChatServerSystem::ChatServerSystem(const core::SystemContext& ctx, ChatServerApplication& app) noexcept:
  System{ctx},
  app_{app},
  adapter_{ctx_.Require<ChatPersistenceSystem>().Require<ChatPersistenceAdapter>()},
  tcp_system_{ctx_.Require<ChatServerTcpSystem>()},
  user_system_{ctx_.Require<UserServerSystem>()},
  scheduling_system_{ctx_.Require<scheduling::SchedulingSystem>()}
{}

void ChatServerSystem::Initialize() {
  app_.RegisterMessageHandler(this, &ChatServerSystem::HandleWriteChatMessage);
  app_.RegisterMessageHandler(this, &ChatServerSystem::HandleCreateChatChannel);
  app_.RegisterMessageHandler(this, &ChatServerSystem::HandleJoinChatChannel);
  app_.RegisterMessageHandler(this, &ChatServerSystem::HandleGetChats);
  app_.RegisterMessageHandler(this, &ChatServerSystem::HandleGetChatChannels);
  app_.RegisterMessageHandler(this, &ChatServerSystem::HandleGetChatChannel);
  app_.RegisterMessageHandler(this, &ChatServerSystem::HandleShutdown);
  app_.RegisterMessageHandler(this, &ChatServerSystem::HandleLogout);

  channel_store_.Prewarm();
  message_store_.Prewarm();

  message_persistence_task_ = scheduling_system_.SchedulePeriodically(1s, [this] { message_store_.Persist(); });
}

void ChatServerSystem::Deinitialize() {
  scheduling_system_.RemoveTask(message_persistence_task_);
  message_store_.Persist();
}

void ChatServerSystem::HandleJoinChatChannel(network::ConnectionId connection_id, const api::JoinChatChannelRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  const auto potential_user = user_system_.GetSessionUser(connection_id);
  if (!potential_user) return;

  const auto potential_channel = channel_store_.GetChannel(message.channel_id);
  if (!potential_channel) {
    app_.HandleRpcError(connection_id, message.request_id, "Channel not found.");
    return;
  }

  const auto& user = potential_user->get();
  const auto potential_previous_channel = channel_store_.GetAssignedChannel(connection_id);
  if (potential_previous_channel) {
    const auto& previous_channel = potential_previous_channel->get();
    ChannelBroadcast<api::ChannelLeaveEventMessage>(previous_channel, { previous_channel.id, user.id});
  }

  channel_store_.AssignConnection(connection_id, message.channel_id);

  const auto& channel = potential_channel->get();
  ChannelBroadcast<api::ChannelJoinEventMessage>(channel, {channel.id, user.id});
  tcp_system_.Send<api::JoinChatChannelResponseMessage>(connection_id, {message.request_id, channel.id});
}

void ChatServerSystem::HandleWriteChatMessage(network::ConnectionId connection_id, const api::WriteChatMessage& message) {
  if (!user_system_.ValidateSession(connection_id)) return;

  const auto potential_user = user_system_.GetSessionUser(connection_id);
  if (!potential_user) {
    tcp_system_.Send<api::ErrorMessage>(connection_id, { "User not found." });
    return;
  }

  const auto potential_channel = channel_store_.GetAssignedChannel(connection_id);
  if (!potential_channel) {
    tcp_system_.Send<api::ErrorMessage>(connection_id, { "No channel joined." });
    return;
  }

  const auto& user = potential_user->get();
  const auto& channel = potential_channel->get();
  message_store_.CreateMessage(channel.id, user.id, message.content);

  auto connections_range = channel_store_.GetConnections(channel.id);
  if (!connections_range) return;

  tcp_system_.Broadcast<api::ReceiveChatMessage>(*connections_range, {user.id, channel.id, message.content});
}

void ChatServerSystem::HandleCreateChatChannel(network::ConnectionId connection_id, const api::CreateChannelRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  const auto potential_user = user_system_.GetSessionUser(connection_id);
  if (!potential_user) return;

  auto channel = adapter_.CreateChatChannel(message.name);
  if (!channel) {
    const auto cached_channel = channel_store_.GetChannel(message.name);
    if (!cached_channel) {
      app_.HandleRpcError(connection_id, message.request_id, "Unable to create channel.");
      return;
    }

    tcp_system_.Send<api::CreateChannelResponseMessage>(connection_id, {message.request_id, *cached_channel});
    return;
  }

  const auto& cached_channel = channel_store_.CacheChannel(std::move(*channel));
  const auto& user = potential_user->get();
  tcp_system_.Send<api::CreateChannelResponseMessage>(connection_id, {message.request_id, cached_channel});
  tcp_system_.Broadcast<api::ChannelCreateEventMessage>({cached_channel, user.id});
}

void ChatServerSystem::HandleShutdown(network::ConnectionId connection_id, const api::ShutdownMessage&) {
  if (!user_system_.ValidateSession(connection_id)) return;
  app_.Quit();
}

void ChatServerSystem::HandleGetChats(network::ConnectionId connection_id, const api::GetChatsRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  constexpr auto max_message_id = std::numeric_limits<api::PersistenceId>::max();
  auto result = message_store_.GetMessagesBefore(message.channel_id, max_message_id, message.limit);
  tcp_system_.Send<api::GetChatsResponseMessage>(connection_id, {message.request_id, message.channel_id, std::move(result)});
}

void ChatServerSystem::HandleGetChatChannels(network::ConnectionId connection_id, const api::GetChatChannelsRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  auto channels = channel_store_.GetChannels();
  tcp_system_.Send<api::GetChatChannelsResponseMessage>(connection_id, {message.request_id, std::move(channels)});
}

void ChatServerSystem::HandleGetChatChannel(network::ConnectionId connection_id, const api::GetChatChannelRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  const auto channel = channel_store_.GetChannel(message.channel_id);
  if (!channel) {
    app_.HandleRpcError(connection_id, message.request_id, "Channel not found.");
    return;
  }

  tcp_system_.Send<api::GetChatChannelResponseMessage>(connection_id, {message.request_id, channel->get()});
}

void ChatServerSystem::HandleLogout(network::ConnectionId connection_id, const api::LogoutRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  user_system_.RemoveSession(connection_id);
  channel_store_.UnassignConnection(connection_id);
  tcp_system_.Send<api::LogoutResponseMessage>(connection_id, {message.request_id});
}

}