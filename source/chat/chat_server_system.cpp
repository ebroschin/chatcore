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

ChatServerSystem::ChatServerSystem(const core::SystemContext& ctx, ChatServerApplication& app):
  System(ctx),
  app_(app),
  adapter_(ctx.Require<ChatPersistenceSystem>().Require<ChatPersistenceAdapter>()),
  tcp_system_(ctx.Require<ChatServerTcpSystem>()),
  user_system_(ctx.Require<UserServerSystem>())
{}

void ChatServerSystem::Initialize() {
  auto& application_system = ctx_.Require<ApplicationSystem>();
  application_system.RegisterMessageHandler<api::PrintMessage>([&](network::ConnectionId id, const api::PrintMessage& message) {
    std::cout << "[client::print " << id << "]" << message.value << std::endl;
  });

  application_system.RegisterMessageHandler<api::WriteChatMessage>([this](network::ConnectionId id, const api::WriteChatMessage& message) {
    WriteChatMessage(id, message.content);
  });

  application_system.RegisterMessageHandler<api::CreateChannelRequestMessage>([this](network::ConnectionId id, const api::CreateChannelRequestMessage& message) {
    CreateChatChannel(id, message.request_id, message.name);
  });

  application_system.RegisterMessageHandler<api::JoinChatChannelRequestMessage>([this](network::ConnectionId id, const api::JoinChatChannelRequestMessage& message) {
    JoinChatChannel(id, message.request_id, message.channel_id);
  });

  application_system.RegisterMessageHandler<api::ShutdownMessage>([this](network::ConnectionId id, const api::ShutdownMessage&) {
    if (!user_system_.ValidateSession(api::ShutdownMessage::TypeId, id)) return;
    app_.Quit();
  });

  application_system.RegisterMessageHandler<api::GetChatsRequestMessage>([this](network::ConnectionId id, const api::GetChatsRequestMessage& message) {
    if (!user_system_.ValidateSession(api::GetChatsRequestMessage::TypeId, id)) return;

    std::cout << "requested chat log for channel: " << message.channel_id << std::endl;

    auto result = message_store_.GetMessagesBefore(message.channel_id, message.max_message_id, message.limit);
    tcp_system_.Send(id, api::GetChatsResponseMessage{message.request_id, message.channel_id, std::move(result)});
  });

  application_system.RegisterMessageHandler<api::GetChatChannelsRequestMessage>([this](network::ConnectionId id, const api::GetChatChannelsRequestMessage& message) {
    if (!user_system_.ValidateSession(api::GetChatChannelsRequestMessage::TypeId, id)) return;

    auto channels = adapter_.GetChatChannels(); //TODO use cache
    tcp_system_.Send(id, api::GetChatChannelsResponseMessage{message.request_id, std::move(channels)});
  });

  message_store_.Prewarm();

  auto& scheduling_system = ctx_.Require<scheduling::SchedulingSystem>();
  scheduling_system.SchedulePeriodically({5s}, [this] { message_store_.Persist(); });
}

void ChatServerSystem::Deinitialize() {
  message_store_.Persist();
  std::cout << "deinitialize chat server system" << std::endl;
}

void ChatServerSystem::JoinChatChannel(network::ConnectionId id, network::RequestId request_id, api::PersistenceId channel_id) {
  if (!user_system_.ValidateSession(request_id, id)) return;

  const auto potential_user = user_system_.GetSessionUser(id);
  if (!potential_user) return;

  const auto channel = adapter_.GetChatChannel(channel_id);
  if (!channel.has_value()) {
    tcp_system_.Send(id, api::ErrorResponseMessage{request_id, "Channel not found."});
    return;
  }

  channel_store_.AssignConnection(id, channel_id);

  const auto& user = potential_user->get();
  const auto previous_channel = channel_store_.GetAssignedChannel(id);
  if (previous_channel) {
    const auto previous_channel_connections = channel_store_.GetConnections(previous_channel->get().id);
    if (previous_channel_connections) {
      tcp_system_.Broadcast<api::PrintMessage>(*previous_channel_connections, { user.name + " left channel " + previous_channel->get().name});
    }
  }

  const auto connections_range = channel_store_.GetConnections(channel_id);
  if (connections_range) {
    tcp_system_.Broadcast<api::PrintMessage>(*connections_range, { user.name + " joined channel " + channel->name });
  }

  tcp_system_.Send(id, api::JoinChatChannelResponseMessage{request_id, channel_id});
}

void ChatServerSystem::WriteChatMessage(network::ConnectionId connection_id, const std::string& content) {
  if (!user_system_.ValidateSession(api::WriteChatMessage::TypeId, connection_id)) return;

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
  message_store_.CreateMessage(channel.id, user.id, content);

  auto connections_range = channel_store_.GetConnections(channel.id);
  if (!connections_range) {
    tcp_system_.Send<api::ErrorMessage>(connection_id, { "No users found in channel." });
    return;
  }

  tcp_system_.Broadcast<api::ReceiveChatMessage>(*connections_range, { user.id, channel.id, content });
}

void ChatServerSystem::CreateChatChannel(network::ConnectionId connection_id, network::RequestId request_id, const std::string& name) const {
  if (!user_system_.ValidateSession(request_id, connection_id)) return;

  const auto potential_user = user_system_.GetSessionUser(connection_id);
  if (!potential_user) return;

  const auto channel = adapter_.CreateChatChannel(name);
  if (!channel) {
    const auto existing_channel = adapter_.GetChatChannel(name);
    if (!existing_channel) return;

    tcp_system_.Send(connection_id, api::CreateChannelResponseMessage{request_id, *existing_channel});
    return;
  }

  std::cout << "created channel: " << name << " with id: " << channel->id << std::endl;

  const auto& user = potential_user->get();
  tcp_system_.Send(connection_id, api::CreateChannelResponseMessage{request_id, *channel});
  tcp_system_.Broadcast<api::PrintMessage>({"[" + channel->name + "] has been created by " + user.name});
}

}