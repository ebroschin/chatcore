#pragma once

#include "../../persistence/persistence_adapter.h"
#include "../../sqlite_persistence/sqlite_persistence_store.h"
#include "chat_persistence_adapter.h"

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

  std::uint32_t CreateChatChannel(const std::string &name) override;
  std::uint32_t CreateChatMessage(const std::uint32_t& channel_id, const api::ChatMessage& message) override;
  std::vector<api::ChatMessage> GetChatMessages(const std::uint32_t& channel_id) override;
};

}