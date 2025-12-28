#pragma once

#include "../../persistence/persistence_adapter.h"
#include <string>
#include <vector>

namespace claw::chat::server {

class ChatPersistenceAdapter : public virtual persistence::PersistenceAdapterBase {
public:
  virtual void CreateChatMessage(std::int64_t channel_id, const std::string& message) = 0;
  virtual void CreateChatChannel(const std::string& name) = 0;
  virtual std::vector<std::string> GetChatMessages(std::int64_t channel_id) = 0;
};

}