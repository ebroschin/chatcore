#include "sqlite_chat_persistence_adapter.h"

namespace claw::chat::server {

void SqliteChatPersistenceAdapter::Initialize() {
  auto& db = store_.Database();
  db.exec("PRAGMA foreign_keys = ON;");

  db.exec("CREATE TABLE IF NOT EXISTS chat_channels (id INTEGER PRIMARY KEY, name TEXT)");
  db.exec(R"(
    CREATE TABLE IF NOT EXISTS chat_messages (
        id INTEGER PRIMARY KEY,
        channel_id INTEGER NOT NULL,
        message TEXT NOT NULL,
        FOREIGN KEY (channel_id) REFERENCES chat_channels(id) ON DELETE CASCADE
    );
  )");
}

void SqliteChatPersistenceAdapter::Deinitialize() {

}

void SqliteChatPersistenceAdapter::CreateChatChannel(const std::string &name) {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_channels (name) VALUES (?);");
  query.bind(1, name);
  query.exec();
}

void SqliteChatPersistenceAdapter::CreateChatMessage(std::int64_t channel_id, const std::string &message) {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_messages (channel_id, message) VALUES (?,?);");
  query.bind(1, channel_id);
  query.bind(2, message);
  query.exec();
}

}