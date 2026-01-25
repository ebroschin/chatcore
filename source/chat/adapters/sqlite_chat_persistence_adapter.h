#pragma once

#include "../../persistence/persistence_adapter.h"
#include "../../sqlite_persistence/sqlite_persistence_store.h"
#include "chat_persistence_adapter.h"
#include <vector>

using namespace claw::persistence::sqlite;

namespace claw::chat::api {
struct ChatMessage;
}

namespace claw::chat::server {

class SqliteChatPersistenceAdapter final : public persistence::PersistenceAdapter<SqlitePersistenceStore, ChatPersistenceAdapter> {
public:
  explicit SqliteChatPersistenceAdapter(SqlitePersistenceStore& store)
  : PersistenceAdapter(store)
  {}

  void Initialize() override;
  void Deinitialize() override;

  std::optional<api::PersistenceId> CreateChatChannel(const std::string &name) override;
  std::optional<api::PersistenceId> GetChatChannel(const std::string& name) override;

  api::PersistenceId CreateChatMessage(const api::PersistenceId& channel_id, const api::ChatMessage& message) override;
  std::vector<api::ChatMessage> GetChatMessages(const api::PersistenceId& channel_id) override;
};

}