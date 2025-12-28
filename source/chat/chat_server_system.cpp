#include "chat_server_system.h"

#include "../communication/tcp_server_system.h"
#include "../prototyping/client_communication_system.h"
#include "chat_persistence_adapter.h"

#include <claw/core/system_context.h>
#include "message_handler/create_chat_message_message_handler.h"
#include "message_handler/create_chat_channel_message_handler.h"

namespace claw::chat::server {

ChatServerSystem::ChatServerSystem(const core::SystemContext& ctx):
  System(ctx),
  adapter_{*ctx.Get<persistence::PersistenceSystemBase>()->Get<ChatPersistenceAdapter>()}
{
  auto* tcp_system = ctx.Get<communication::TCPServerSystemBase>();
  tcp_system->RegisterMessageHandler<CreateChatChannelMessageHandler>("create_chat_channel", *this);
  tcp_system->RegisterMessageHandler<CreateChatMessageMessageHandler>("create_chat_message", *this);
}

void ChatServerSystem::CreateChatChannel(const std::string& name) {
  adapter_.CreateChatChannel(name);
}

void ChatServerSystem::CreateChatMessage(std::int64_t channel_id, const std::string& message) {
  adapter_.CreateChatMessage(channel_id, message);
}

}