#pragma once

#include "chat_channel_message_log.h"

#include <claw/chat/api.h>
#include <mutex>
#include <optional>
#include <queue>
#include <unordered_map>

namespace claw::chat::server {

class ChatPersistenceAdapter;

class ChatMessageStore {
public:
  explicit ChatMessageStore(ChatPersistenceAdapter& adapter);

  void Prewarm();
  void CreateMessage(api::PersistenceId channel_id, api::PersistenceId user_id, const std::string& message);
  void CacheMessage(api::ChatMessage chat_message);
  void CacheMessages(api::PersistenceId channel_id, std::vector<api::ChatMessage> chat_message);
  void Persist();

  std::optional<std::reference_wrapper<const api::ChatMessage>>
  GetMessage(api::PersistenceId message_id);

  std::vector<api::ChatMessage>
  GetMessagesBefore(api::PersistenceId channel_id, api::PersistenceId message_id, std::uint32_t limit);

private:
  ChatPersistenceAdapter& adapter_;

  std::mutex persist_mutex_; //TODO rename to mutex and make the entire class thread safe
  std::optional<api::PersistenceId> latest_persisted_id_{std::nullopt};
  api::PersistenceId next_id_{0};

  std::unordered_map<api::PersistenceId, api::ChatMessage> message_cache_{};
  std::queue<api::PersistenceId> pending_message_ids_{};
  std::unordered_map<api::PersistenceId, ChatChannelMessageLog> message_logs_{};
};

}

