#pragma once

#include <claw/chat/api.h>
#include "../../persistence/persistence_adapter.h"
#include <string>
#include <vector>
#include <optional>
#include <span>

namespace claw::chat::server {

class ChatPersistenceAdapter : public virtual persistence::PersistenceAdapterBase {
public:
  virtual std::optional<api::ChatChannel> CreateChatChannel(const std::string& name) = 0;
  virtual std::optional<api::ChatChannel> GetChatChannel(api::PersistenceId id) = 0;
  virtual std::optional<api::ChatChannel> GetChatChannel(const std::string& name) = 0;
  virtual std::vector<api::ChatChannel> GetChatChannels() = 0;
  virtual std::optional<api::PersistenceId> PersistChatMessages(std::span<const api::ChatMessage> messages) = 0;

  virtual std::optional<api::ChatMessage>
  CreateChatMessage(api::PersistenceId channel_id, api::PersistenceId user_id, const std::string& content) = 0;

  virtual std::vector<api::ChatMessage>
  GetChatMessagesBefore(api::PersistenceId channel_id, api::PersistenceId message_id, std::uint32_t limit) = 0;

  virtual std::optional<api::PersistenceId> GetFirstChatMessageId(api::PersistenceId channel_id) = 0;
  virtual std::optional<api::PersistenceId> GetLastChatMessageId() = 0;
};

}