#pragma once

#include <ebroschin/chat/api.hpp>

namespace ebroschin::chatcore::server {

class ChatMessageStore;
class ChatPersistenceAdapter;

class ChatChannelMessageLog {
public:
  explicit ChatChannelMessageLog(api::PersistenceId channel_id, ChatMessageStore& store, ChatPersistenceAdapter& adapter);

  void Prewarm() const;
  void AppendMessageId(api::PersistenceId message_id);
  void AssignMessageIds(std::vector<api::PersistenceId> message_ids);

  std::vector<api::PersistenceId> GetLatestChatMessages(std::uint32_t limit);
  std::vector<api::PersistenceId> GetChatMessagesBefore(api::PersistenceId message_id, std::uint32_t limit);

private:
  static constexpr std::uint32_t PrewarmMessageCount = 100;

  std::vector<api::PersistenceId> QueryAndCacheMessages(api::PersistenceId message_id, std::uint32_t limit);

  api::PersistenceId channel_id_;
  ChatMessageStore& store_;
  ChatPersistenceAdapter& adapter_;

  //invariant: unique and sorted ascending
  std::vector<api::PersistenceId> channel_message_ids_{};

  bool cache_complete_{false};
};

}