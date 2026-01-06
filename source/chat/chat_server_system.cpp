#include "chat_server_system.h"

#include "../communication/tcp_system.h"
#include "adapters/chat_persistence_adapter.h"
#include "../application/chat_persistence_system.h"
#include <iostream>

#include <claw/core/system_context.h>

namespace claw::chat::server {

ChatServerSystem::ChatServerSystem(const core::SystemContext& ctx):
  System(ctx),
  adapter_{*ctx.Get<ChatPersistenceSystem>()->Get<ChatPersistenceAdapter>()},
  tcp_system_{*ctx.Get<ChatServerTcpSystem>()}
{
  tcp_system_.RegisterMessageHandler<communication::TestMessage>([&](const communication::TestMessage& message) {
    std::cout << "message from client received: " << message.value << std::endl;
  });

  tcp_system_.RegisterMessageHandler<api::WriteChatMessage>([&](const api::WriteChatMessage& message) {
    const auto id = CreateChatMessage(message.channel_id, message.message);
    std::cout << "created message with id: " << id << std::endl;
  });

  tcp_system_.RegisterMessageHandler<api::CreateChannelMessage>([&](const api::CreateChannelMessage& message) {
    const auto id = CreateChatChannel(message.name);
    std::cout << "created channel: " << message.name << "with id: " << id << std::endl;
  });

  tcp_system_.RegisterMessageHandler<api::GetChatsRequestMessage>([&](const api::GetChatsRequestMessage& message) {
    std::cout << "requested chat log for channel: " << message.channel_id << std::endl;
    auto result = GetChatMessages(message.channel_id);
    tcp_system_.SendMessage<api::GetChatsResponseMessage>({message.channel_id, std::move(result)});
  });
}

std::uint32_t ChatServerSystem::CreateChatChannel(const std::string& name) {
  return adapter_.CreateChatChannel(name);
}

std::uint32_t ChatServerSystem::CreateChatMessage(const std::uint32_t& channel_id, const api::ChatMessage& message) {
  return adapter_.CreateChatMessage(channel_id, message);
}

std::vector<api::ChatMessage> ChatServerSystem::GetChatMessages(const std::uint32_t& channel_id) {
  return adapter_.GetChatMessages(channel_id);
}

}