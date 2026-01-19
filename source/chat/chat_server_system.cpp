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
{

}

void ChatServerSystem::Initialize() {
  user_system_ = ctx_.Get<UserServerSystem>();

  tcp_system_.RegisterMessageHandler<api::PrintMessage>([&](network::ConnectionId id, const api::PrintMessage& message) {
    std::cout << "[" << id << "]" <<  " message from client received: " << message.value << std::endl;
  });

  tcp_system_.RegisterMessageHandler<api::WriteChatMessage>([&](network::ConnectionId id, const api::WriteChatMessage& message) {
    if (!user_system_->ValidateSession(id)) return;
    const auto message_id = CreateChatMessage(message.channel_id, message.message);
    std::cout << "created message with id: " << message_id << std::endl;

    auto user = user_system_->GetSessionUser(id);
    if (!user.has_value()) return;

    tcp_system_.Broadcast<api::PrintMessage>({"[" + std::to_string(message.channel_id) + "]" + user.value().get().name + " says: " + message.message.content });
  });

  tcp_system_.RegisterMessageHandler<api::CreateChannelMessage>([&](network::ConnectionId id, const api::CreateChannelMessage& message) {
    if (!user_system_->ValidateSession(id)) return;
    const auto channel_id = CreateChatChannel(message.name);
    std::cout << "created channel: " << message.name << "with id: " << channel_id << std::endl;

    auto user = user_system_->GetSessionUser(id);
    if (!user.has_value()) return;

    tcp_system_.Broadcast<api::PrintMessage>({"[" + std::to_string(channel_id) + "] has been created by" + user.value().get().name});
  });

  tcp_system_.RegisterMessageHandler<api::GetChatsRequestMessage>([&](network::ConnectionId id, const api::GetChatsRequestMessage& message) {
    if (!user_system_->ValidateSession(id)) return;
    std::cout << "requested chat log for channel: " << message.channel_id << std::endl;

    auto result = GetChatMessages(message.channel_id);
    tcp_system_.Send<api::GetChatsResponseMessage>(id, {message.channel_id, std::move(result)});
  });
}

api::PersistenceID ChatServerSystem::CreateChatChannel(const std::string& name) {
  return adapter_.CreateChatChannel(name);
}

api::PersistenceID ChatServerSystem::CreateChatMessage(const api::PersistenceID& channel_id, const api::ChatMessage& message) {
  return adapter_.CreateChatMessage(channel_id, message);
}

std::vector<api::ChatMessage> ChatServerSystem::GetChatMessages(const api::PersistenceID& channel_id) {
  return adapter_.GetChatMessages(channel_id);
}

}