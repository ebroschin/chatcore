#include "chat_server_system.h"

#include "../application/application_system.h"

#include <claw/scheduling/scheduling_system.h>

#include "../application/chat_persistence_system.h"
#include "../application/chat_server_application.h"
#include "adapters/chat_persistence_adapter.h"
#include <claw/network/tcp/tcp_system.h>

#include <iostream>

#include "../users/user_server_system.h"
#include <claw/core/system_context.h>

using namespace std::chrono_literals;

namespace claw::chat::server {

ChatServerSystem::ChatServerSystem(const core::SystemContext& ctx):
  System(ctx),
  app_system_(ctx.Require<ApplicationSystem>()),
  adapter_(ctx.Require<ChatPersistenceSystem>().Require<ChatPersistenceAdapter>()),
  tcp_system_(ctx.Require<ChatServerTcpSystem>()),
  user_system_(ctx.Require<UserServerSystem>())
{}

void ChatServerSystem::Initialize() {
  RegisterMessageHandler(&ChatServerSystem::HandleWriteChatMessage);
  RegisterMessageHandler(&ChatServerSystem::HandleCreateChatChannel);
  RegisterMessageHandler(&ChatServerSystem::HandleJoinChatChannel);
  RegisterMessageHandler(&ChatServerSystem::HandleGetChats);
  RegisterMessageHandler(&ChatServerSystem::HandleGetChatChannels);
  RegisterMessageHandler(&ChatServerSystem::HandleShutdown);

  channel_store_.Prewarm();
  message_store_.Prewarm();

  auto& scheduling_system = ctx_.Require<scheduling::SchedulingSystem>();
  scheduling_system.SchedulePeriodically({5s}, [this] { message_store_.Persist(); });
}

void ChatServerSystem::Deinitialize() {
  message_store_.Persist();
}

void ChatServerSystem::HandleJoinChatChannel(network::ConnectionId connection_id, const api::JoinChatChannelRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  const auto potential_user = user_system_.GetSessionUser(connection_id);
  if (!potential_user) return;

  const auto channel = adapter_.GetChatChannel(message.channel_id);
  if (!channel.has_value()) {
    tcp_system_.Send(connection_id, api::ErrorResponseMessage{message.request_id, "Channel not found."});
    return;
  }

  const auto& user = potential_user->get();
  const auto previous_channel = channel_store_.GetAssignedChannel(connection_id);
  if (previous_channel) {
    const auto previous_channel_connections = channel_store_.GetConnections(previous_channel->get().id);
    if (previous_channel_connections) {
      tcp_system_.Broadcast<api::PrintMessage>(*previous_channel_connections, { user.name + " left channel " + previous_channel->get().name});
    }
  }

  channel_store_.AssignConnection(connection_id, message.channel_id);

  const auto connections_range = channel_store_.GetConnections(message.channel_id);
  if (connections_range) {
    tcp_system_.Broadcast<api::PrintMessage>(*connections_range, { user.name + " joined channel " + channel->name });
  }

  tcp_system_.Send(connection_id, api::JoinChatChannelResponseMessage{message.request_id, message.channel_id});
}

void ChatServerSystem::HandleWriteChatMessage(network::ConnectionId connection_id, const api::WriteChatMessage& message) {
  if (!user_system_.ValidateSession(connection_id)) return;

  const auto potential_user = user_system_.GetSessionUser(connection_id);
  if (!potential_user.has_value()) {
    tcp_system_.Send<api::ErrorMessage>(connection_id, { "User not found." });
    return;
  }

  const auto potential_channel = channel_store_.GetAssignedChannel(connection_id);
  if (!potential_channel.has_value()) {
    tcp_system_.Send<api::ErrorMessage>(connection_id, { "No channel joined." });
    return;
  }

  const auto& user = potential_user->get();
  const auto& channel = potential_channel->get();
  message_store_.CreateMessage(channel.id, user.id, message.content);

  auto connections_range = channel_store_.GetConnections(channel.id);
  if (!connections_range) {
    tcp_system_.Send<api::ErrorMessage>(connection_id, { "No users found in channel." });
    return;
  }

  tcp_system_.Broadcast<api::ReceiveChatMessage>(*connections_range, { user.id, channel.id, message.content });
}

void ChatServerSystem::HandleCreateChatChannel(network::ConnectionId connection_id, const api::CreateChannelRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  const auto potential_user = user_system_.GetSessionUser(connection_id);
  if (!potential_user) return;

  auto channel = adapter_.CreateChatChannel(message.name);
  if (!channel) {
    const auto cached_channel = channel_store_.GetChannel(message.name);
    tcp_system_.Send(connection_id, api::CreateChannelResponseMessage{message.request_id, *cached_channel});
    return;
  }

  const auto& cached_channel = channel_store_.CacheChannel(std::move(*channel));
  const auto& user = potential_user->get();
  tcp_system_.Send(connection_id, api::CreateChannelResponseMessage{message.request_id, cached_channel});
  tcp_system_.Broadcast<api::PrintMessage>({"[" + channel->name + "] has been created by " + user.name});
}

void ChatServerSystem::HandleShutdown(network::ConnectionId connection_id, const api::ShutdownMessage&) {
  if (!user_system_.ValidateSession(connection_id)) return;
  app_system_.Shutdown();
}

void ChatServerSystem::HandleGetChats(network::ConnectionId connection_id, const api::GetChatsRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  auto result = message_store_.GetMessagesBefore(message.channel_id, message.max_message_id, message.limit);
  tcp_system_.Send(connection_id, api::GetChatsResponseMessage{message.request_id, message.channel_id, std::move(result)});
}

void ChatServerSystem::HandleGetChatChannels(network::ConnectionId connection_id, const api::GetChatChannelsRequestMessage& message) {
  if (!user_system_.ValidateSession(message.request_id, connection_id)) return;

  auto channels = channel_store_.GetChannels();
  tcp_system_.Send(connection_id, api::GetChatChannelsResponseMessage{message.request_id, std::move(channels)});
}

}