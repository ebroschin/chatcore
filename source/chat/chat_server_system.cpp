#include "chat_server_system.h"

#include "../sqlite_persistence/sqlite_persistence_store.h"
#include "chat_persistence_adapter.h"
#include <claw/core/system_context.h>

namespace claw::chat::server {

ChatServerSystem::ChatServerSystem(const core::SystemContext& ctx):
  System(ctx),
  adapter_{*ctx.Get<persistence::PersistenceSystemBase>()->Get<ChatPersistenceAdapter>()}
{}

void ChatServerSystem::CreateChatChannel(const std::string& name) {
  adapter_.CreateChatChannel(name);
}

void ChatServerSystem::CreateChatMessage(std::int64_t channel_id, const std::string& message) {
  adapter_.CreateChatMessage(channel_id, message);
}

}