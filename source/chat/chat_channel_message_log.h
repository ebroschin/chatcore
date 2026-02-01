#pragma once

#include <claw/chat/api.h>
#include <optional>

namespace claw::chat::server {

class ChatMessageStore;
class ChatPersistenceAdapter;

class ChatChannelMessageLog {
public:
  explicit ChatChannelMessageLog(api::PersistenceId channel_id, ChatMessageStore& store, ChatPersistenceAdapter& adapter);

  void Prewarm();
  void AppendMessageId(api::PersistenceId message_id);
  void AssignMessageIds(std::vector<api::PersistenceId> message_ids);

  std::vector<api::PersistenceId> GetLatestChatMessages(std::uint32_t limit);
  std::vector<api::PersistenceId> GetChatMessagesBefore(api::PersistenceId message_id, std::uint32_t limit);

private:
  std::vector<api::PersistenceId> QueryAndCacheMessages(api::PersistenceId message_id, std::uint32_t limit);

  api::PersistenceId channel_id_;
  ChatMessageStore& store_;
  ChatPersistenceAdapter& adapter_;

  //invariant: unique and sorted ascending
  std::vector<api::PersistenceId> channel_message_ids_{};

  std::optional<api::PersistenceId> first_channel_message_id_{std::nullopt};
  //api::PersistenceId min_message_id_{};
  //api::PersistenceId max_message_id_{};
};

}