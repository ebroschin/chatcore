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

  std::optional<api::ChatChannel> CreateChatChannel(const std::string &name) override;
  std::optional<api::ChatChannel> GetChatChannel(const std::string& name) override;
  std::optional<api::ChatChannel> GetChatChannel(api::PersistenceId id) override;

  std::optional<api::ChatMessage> CreateChatMessage(api::PersistenceId channel_id, api::PersistenceId user_id, const std::string& content) override;
  std::vector<api::ChatMessage> GetChatMessages(api::PersistenceId channel_id) override;
};

}