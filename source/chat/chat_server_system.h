#pragma once

#include "../persistence/persistence_system.h"

#include <claw/core/system.h>
#include <string>

namespace claw::chat::server {

class ChatPersistenceAdapter;

class ChatServerSystem final : public core::System {
public:
  explicit ChatServerSystem(const core::SystemContext& ctx);

  void CreateChatChannel(const std::string& name);
  void CreateChatMessage(std::int64_t channel_id, const std::string& message);

private:
  ChatPersistenceAdapter& adapter_;
};

}