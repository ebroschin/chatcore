#pragma once

#include "../sqlite_persistence/sqlite_persistence_store.h"
#include "chat_persistence_adapter.h"

using namespace claw::persistence::sqlite;

namespace claw::chat::server {

class SqliteChatPersistenceAdapter final : public ChatPersistenceAdapter {
public:
  explicit SqliteChatPersistenceAdapter(SqlitePersistenceStore* store)
  : ChatPersistenceAdapter(store)
  {}

  void CreateChatChannel(const std::string &name) override;
  void CreateChatMessage(std::int64_t channel_id, const std::string &message) override;
};

}