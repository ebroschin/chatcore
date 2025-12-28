#include "chat_server_system.h"

#include <claw/core/system_context.h>
#include "chat_persistence_adapter.h"

namespace claw::chat::server {

ChatServerSystem::ChatServerSystem(const core::SystemContext& ctx):
  System(ctx),
  adapter_{ctx.Get<persistence::PersistenceSystemBase>()->GetAdapter<ChatPersistenceAdapter>()}
{}

void ChatServerSystem::CreateMessage(std::int64_t channel_id, const std::string& message) {
  adapter_->CreateChatMessage(channel_id, message);
}

}