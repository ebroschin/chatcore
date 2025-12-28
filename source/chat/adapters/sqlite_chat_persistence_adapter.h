#pragma once

#include "../../persistence/persistence_adapter.h"
#include "../../sqlite_persistence/sqlite_persistence_store.h"
#include "chat_persistence_adapter.h"

using namespace claw::persistence::sqlite;

namespace claw::chat::server {

class SqliteChatPersistenceAdapter final : public persistence::PersistenceAdapter<SqlitePersistenceStore, ChatPersistenceAdapter> {
public:
  explicit SqliteChatPersistenceAdapter(SqlitePersistenceStore& store)
  : PersistenceAdapter(store)
  {}

  void Initialize() override;
  void Deinitialize() override;

  void CreateChatChannel(const std::string &name) override;
  void CreateChatMessage(std::int64_t channel_id, const std::string &message) override;
  std::vector<std::string> GetChatMessages(std::int64_t channel_id) override;
};

}