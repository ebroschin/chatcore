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
}

void ChatServerSystem::CreateChatChannel(const std::string& name) {
  adapter_.CreateChatChannel(name);
}

void ChatServerSystem::CreateChatMessage(const std::int64_t& channel_id, const std::string& message) {
  adapter_.CreateChatMessage(channel_id, message);
}

std::vector<std::string> ChatServerSystem::GetChatMessages(const std::int64_t& channel_id) {
  return adapter_.GetChatMessages(channel_id);
}

}