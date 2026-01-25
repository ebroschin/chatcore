#pragma once

#include "../application/chat_tcp_system.h"

#include <claw/core/system.h>
#include <string>
#include <vector>

namespace claw::chat::server {

class ChatPersistenceAdapter;
class UserServerSystem;

class ChatServerSystem final : public core::System {
public:
  explicit ChatServerSystem(const core::SystemContext& ctx);

  void Initialize() override;

  api::PersistenceId CreateChatMessage(const api::PersistenceId& channel_id, const api::ChatMessage& message);
  std::optional<api::PersistenceId> CreateChatChannel(const std::string& name);
  std::vector<api::ChatMessage> GetChatMessages(const api::PersistenceId& channel_id);
  
private:
  ChatPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;
  UserServerSystem* user_system_;
};

}