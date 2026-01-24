#include "sqlite_chat_persistence_adapter.h"
#include <claw/chat/api.h>

namespace claw::chat::server {

void SqliteChatPersistenceAdapter::Initialize() {
  auto& db = store_.Database();
  db.exec("PRAGMA foreign_keys = ON;");

  db.exec("CREATE TABLE IF NOT EXISTS chat_channels (id INTEGER PRIMARY KEY, name TEXT)");
  db.exec(R"(
    CREATE TABLE IF NOT EXISTS chat_messages (
        id INTEGER PRIMARY KEY,
        channel_id INTEGER NOT NULL,
        user_id INTEGER NOT NULL,
        message TEXT NOT NULL,
        FOREIGN KEY (channel_id) REFERENCES chat_channels(id) ON DELETE CASCADE
    );
  )");
}

void SqliteChatPersistenceAdapter::Deinitialize() {

}

std::uint32_t SqliteChatPersistenceAdapter::CreateChatChannel(const std::string &name) {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_channels (name) VALUES (?);");
  query.bind(1, name);
  query.exec();

  return static_cast<std::uint32_t>(store_.Database().getLastInsertRowid());
}

std::uint32_t SqliteChatPersistenceAdapter::CreateChatMessage(const std::uint32_t& channel_id, const api::ChatMessage& message) {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_messages (channel_id, user_id, message) VALUES (?,?,?);");
  query.bind(1, channel_id);
  query.bind(2, message.user_id);
  query.bind(3, message.content);
  query.exec();

  return static_cast<std::uint32_t>(store_.Database().getLastInsertRowid());
}

std::vector<api::ChatMessage> SqliteChatPersistenceAdapter::GetChatMessages(const std::uint32_t& channel_id) {
  std::vector<api::ChatMessage> result;
  const auto& db = store_.Database();

  SQLite::Statement query(db,
      "SELECT user_id, message FROM chat_messages "
      "WHERE channel_id = ? "
      "ORDER BY id ASC;"
  );

  query.bind(1, channel_id);

  while (query.executeStep()) {
    auto user_id = query.getColumn(0).getUInt();
    auto content = query.getColumn(1).getString();
    result.push_back({user_id, content});
  }

  return result;
}

}