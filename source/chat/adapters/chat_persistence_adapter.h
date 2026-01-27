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
  virtual std::optional<api::ChatChannel> CreateChatChannel(const std::string& name) = 0;
  virtual std::optional<api::ChatChannel> GetChatChannel(api::PersistenceId id) = 0;
  virtual std::optional<api::ChatChannel> GetChatChannel(const std::string& name) = 0;

  virtual std::optional<api::ChatMessage> CreateChatMessage(api::PersistenceId channel_id, api::PersistenceId user_id, const std::string& content) = 0;
  virtual std::vector<api::ChatMessage> GetChatMessages(api::PersistenceId channel_id) = 0;
};

}