#include "sqlite_chat_persistence_adapter.h"
#include <claw/chat/api.h>

namespace claw::chat::server {

void SqliteChatPersistenceAdapter::Initialize() {
  auto& db = store_.Database();
  db.exec("PRAGMA foreign_keys = ON;");

  db.exec("CREATE TABLE IF NOT EXISTS chat_channels (id INTEGER PRIMARY KEY, name TEXT NOT NULL UNIQUE)");
  db.exec(R"(
    CREATE TABLE IF NOT EXISTS chat_messages (
        id INTEGER PRIMARY KEY,
        channel_id INTEGER NOT NULL,
        user_id INTEGER NOT NULL,
        content TEXT NOT NULL,
        FOREIGN KEY (channel_id) REFERENCES chat_channels(id) ON DELETE CASCADE
    );
  )");
}

void SqliteChatPersistenceAdapter::Deinitialize() { }

std::optional<api::ChatChannel> SqliteChatPersistenceAdapter::CreateChatChannel(const std::string &name) {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_channels (name) VALUES (?) ON CONFLICT(name) DO NOTHING;");
  query.bind(1, name);

  if (query.exec() <= 0) return std::nullopt;

  auto id = static_cast<api::PersistenceId>(store_.Database().getLastInsertRowid());
  return api::ChatChannel{id, name};
}

std::optional<api::ChatMessage> SqliteChatPersistenceAdapter::CreateChatMessage(api::PersistenceId channel_id, api::PersistenceId user_id, const std::string& content) {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_messages (channel_id, user_id, content) VALUES (?,?,?);");
  query.bind(1, channel_id);
  query.bind(2, user_id);
  query.bind(3, content);

  if (query.exec() <= 0) return std::nullopt;

  auto id = static_cast<api::PersistenceId>(store_.Database().getLastInsertRowid());
  return api::ChatMessage{id, channel_id, user_id, content};
}

std::vector<api::ChatMessage> SqliteChatPersistenceAdapter::GetChatMessages(api::PersistenceId channel_id) {
  std::vector<api::ChatMessage> result;
  const auto& db = store_.Database();

  SQLite::Statement query(db,
      "SELECT id, user_id, content FROM chat_messages "
      "WHERE channel_id = ? "
      "ORDER BY id ASC;"
  );

  query.bind(1, channel_id);

  while (query.executeStep()) {
    auto id = query.getColumn(0).getUInt();
    auto user_id = query.getColumn(1).getUInt();
    auto content = query.getColumn(2).getString();
    result.push_back({id, channel_id, user_id, content});
  }

  return result;
}

std::optional<api::ChatChannel> SqliteChatPersistenceAdapter::GetChatChannel(const std::string& name) {
  const auto& db = store_.Database();
  SQLite::Statement query(db,
      "SELECT id FROM chat_channels "
      "WHERE name = ? "
      "LIMIT 1;"
  );

  query.bind(1, name);

  while (query.executeStep()) {
    auto id = query.getColumn(0).getUInt();
    return api::ChatChannel{id, name};
  }

  return std::nullopt;
}

std::optional<api::ChatChannel> SqliteChatPersistenceAdapter::GetChatChannel(api::PersistenceId id) {
  const auto& db = store_.Database();
  SQLite::Statement query(db,
      "SELECT name FROM chat_channels "
      "WHERE id = ? "
      "LIMIT 1;"
  );

  query.bind(1, id);

  while (query.executeStep()) {
    auto name = query.getColumn(0).getString();
    return api::ChatChannel{id, std::move(name)};
  }

  return std::nullopt;
}

}