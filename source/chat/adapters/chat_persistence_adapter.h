#pragma once

#include <claw/chat/api.h>
#include "../../persistence/persistence_adapter.h"
#include <string>
#include <vector>

namespace claw::chat::api {
struct ChatMessage;
}

namespace claw::chat::server {

class ChatPersistenceAdapter : public virtual persistence::PersistenceAdapterBase {
public:
  virtual api::PersistenceId CreateChatMessage(const api::PersistenceId& channel_id, const api::ChatMessage& message) = 0;
  virtual std::optional<api::PersistenceId> CreateChatChannel(const std::string& name) = 0;
  virtual std::optional<api::PersistenceId> GetChatChannel(const std::string& name) = 0;
  virtual std::vector<api::ChatMessage> GetChatMessages(const api::PersistenceId& channel_id) = 0;
};

}