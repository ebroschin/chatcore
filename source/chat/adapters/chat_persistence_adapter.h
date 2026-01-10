#pragma once

#include "../../application/commons.h"
#include "../../persistence/persistence_adapter.h"
#include <string>
#include <vector>

namespace claw::chat::api {
struct ChatMessage;
}

namespace claw::chat::server {

class ChatPersistenceAdapter : public virtual persistence::PersistenceAdapterBase {
public:
  virtual api::PersistenceID CreateChatMessage(const api::PersistenceID& channel_id, const api::ChatMessage& message) = 0;
  virtual api::PersistenceID CreateChatChannel(const std::string& name) = 0;
  virtual std::vector<api::ChatMessage> GetChatMessages(const api::PersistenceID& channel_id) = 0;
};

}