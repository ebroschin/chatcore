#pragma once

#include "../../persistence/persistence_adapter.h"
#include <string>
#include <vector>

namespace claw::chat::api {
struct ChatMessage;
}

namespace claw::chat::server {

class ChatPersistenceAdapter : public virtual persistence::PersistenceAdapterBase {
public:
  virtual std::uint32_t CreateChatMessage(const std::uint32_t& channel_id, const api::ChatMessage& message) = 0;
  virtual std::uint32_t CreateChatChannel(const std::string& name) = 0;
  virtual std::vector<api::ChatMessage> GetChatMessages(const std::uint32_t& channel_id) = 0;
};

}