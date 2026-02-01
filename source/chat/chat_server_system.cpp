#include "chat_server_system.h"

#include <claw/network/tcp/tcp_system.h>
#include "adapters/chat_persistence_adapter.h"
#include "../application/chat_persistence_system.h"
#include <iostream>

#include "../users/user_server_system.h"
#include <claw/core/system_context.h>

namespace claw::chat::server {

ChatServerSystem::ChatServerSystem(const core::SystemContext& ctx):
  System(ctx),
  adapter_{*ctx.Get<ChatPersistenceSystem>()->Get<ChatPersistenceAdapter>()},
  tcp_system_{*ctx.Get<ChatServerTcpSystem>()}
{}

void ChatServerSystem::Initialize() {
  user_system_ = ctx_.Get<UserServerSystem>();

  tcp_system_.RegisterMessageHandler<api::PrintMessage>([&](network::ConnectionId id, const api::PrintMessage& message) {
    std::cout << "[client::print " << id << "]" << message.value << std::endl;
  });

  tcp_system_.RegisterMessageHandler<api::WriteChatMessage>([this](network::ConnectionId id, const api::WriteChatMessage& message) {
    WriteChatMessage(id, message.content);
  });

  tcp_system_.RegisterMessageHandler<api::CreateChannelRequestMessage>([this](network::ConnectionId id, const api::CreateChannelRequestMessage& message) {
    CreateChatChannel(id, message.name);
  });

  tcp_system_.RegisterMessageHandler<api::JoinChatChannelRequestMessage>([this](network::ConnectionId id, const api::JoinChatChannelRequestMessage& message) {
    JoinChatChannel(id, message.channel_id);
  });

  tcp_system_.RegisterMessageHandler<api::GetChatsRequestMessage>([this](network::ConnectionId id, const api::GetChatsRequestMessage& message) {
    if (!user_system_->ValidateSession(api::GetChatsRequestMessage::TypeId, id)) return;

    std::cout << "requested chat log for channel: " << message.channel_id << std::endl;

    auto result = message_store_.GetMessagesBefore(message.channel_id, message.max_message_id, message.limit);
    tcp_system_.Send<api::GetChatsResponseMessage>(id, {message.channel_id, std::move(result)});
  });

  message_store_.Prewarm();
}

void ChatServerSystem::JoinChatChannel(network::ConnectionId id, api::PersistenceId channel_id) {
  if (!user_system_->ValidateSession(api::JoinChatChannelRequestMessage::TypeId, id)) return;

  auto potential_user = user_system_->GetSessionUser(id);
  if (!potential_user) return;

  auto channel = adapter_.GetChatChannel(channel_id);
  if (!channel.has_value()) {
    tcp_system_.Send<api::ErrorMessage>(id, {api::WriteChatMessage::TypeId, "Channel not found."});
    return;
  }

  const auto& user = potential_user->get();
  auto previous_channel = channel_store_.GetAssignedChannel(id);
  if (previous_channel) {
    std::cout << user.name << " left channel " << previous_channel->get().name << std::endl;
  }

  channel_store_.AssignConnection(id, channel_id);

  std::cout << user.name << " joined channel " << channel->name << std::endl;
  tcp_system_.Send<api::JoinChatChannelResponseMessage>(id, {channel_id});
}

void ChatServerSystem::WriteChatMessage(network::ConnectionId connection_id, const std::string& content) {
  if (!user_system_->ValidateSession(api::WriteChatMessage::TypeId, connection_id)) return;

  auto potential_user = user_system_->GetSessionUser(connection_id);
  if (!potential_user.has_value()) {
    tcp_system_.Send<api::ErrorMessage>(connection_id, {api::WriteChatMessage::TypeId, "User not found."});
    return;
  }

  auto potential_channel = channel_store_.GetAssignedChannel(connection_id);
  if (!potential_channel.has_value()) {
    tcp_system_.Send<api::ErrorMessage>(connection_id, {api::WriteChatMessage::TypeId, "No channel joined."});
    return;
  }

  const auto& user = potential_user->get();
  const auto& channel = potential_channel->get();
  message_store_.CreateMessage(channel.id, user.id, content);

  auto connections_range = channel_store_.GetConnections(channel.id);
  if (!connections_range) {
    tcp_system_.Send<api::ErrorMessage>(connection_id, {api::WriteChatMessage::TypeId, "No users found in channel."});
    return;
  }

  tcp_system_.Broadcast<api::PrintMessage>(*connections_range, {"[" + channel.name + "] " + user.name + " says: " + content });
}

void ChatServerSystem::CreateChatChannel(network::ConnectionId connection_id, const std::string& name) {
  if (!user_system_->ValidateSession(api::CreateChannelRequestMessage::TypeId, connection_id)) return;

  auto potential_user = user_system_->GetSessionUser(connection_id);
  if (!potential_user) return;

  auto channel = adapter_.CreateChatChannel(name);
  if (!channel) {
    auto existing_channel = adapter_.GetChatChannel(name);
    if (!existing_channel) return;

    tcp_system_.Send<api::CreateChannelResponseMessage>(connection_id, {*existing_channel});
    return;
  }

  std::cout << "created channel: " << name << " with id: " << channel->id << std::endl;

  const auto& user = potential_user->get();
  tcp_system_.Send<api::CreateChannelResponseMessage>(connection_id, {*channel});
  tcp_system_.Broadcast<api::PrintMessage>({"[" + channel->name + "] has been created by " + user.name});
}

}