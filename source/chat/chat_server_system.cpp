#include "chat_server_system.h"

#include "../communication/tcp_system.h"
#include "adapters/chat_persistence_adapter.h"

#include "message_handler/create_chat_channel_message_handler.h"
#include "message_handler/create_chat_message_message_handler.h"
#include "message_handler/get_chat_messages_message_handler.h"

#include <claw/core/system_context.h>

namespace claw::chat::server {

ChatServerSystem::ChatServerSystem(const core::SystemContext& ctx):
  System(ctx),
  adapter_{*ctx.Get<persistence::PersistenceSystemBase>()->Get<ChatPersistenceAdapter>()},
  tcp_system_{*ctx.Get<communication::TcpSystem>()}
{
  tcp_system_.RegisterMessageHandler<CreateChatChannelMessageHandler>("create_chat_channel", *this);
  tcp_system_.RegisterMessageHandler<CreateChatMessageMessageHandler>("create_chat_message", *this);
  tcp_system_.RegisterMessageHandler<GetChatMessagesMessageHandler>("get_chat_messages", *this);
}

void ChatServerSystem::CreateChatChannel(const std::string& name) {
  adapter_.CreateChatChannel(name);
}

void ChatServerSystem::CreateChatMessage(std::int64_t channel_id, const std::string& message) {
  adapter_.CreateChatMessage(channel_id, message);
}

std::vector<std::string> ChatServerSystem::GetChatMessages(std::int64_t channel_id) {
  return adapter_.GetChatMessages(channel_id);
}

}