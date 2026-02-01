#include "sqlite_chat_persistence_adapter.h"
#include <claw/chat/api.h>

namespace claw::chat::server {

void SqliteChatPersistenceAdapter::Initialize() {
  auto& db = store_.Database();
  db.exec("PRAGMA foreign_keys = ON;");
  db.exec("PRAGMA synchronous=NORMAL;");
  db.exec("PRAGMA journal_mode=WAL;");

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

std::optional<api::PersistenceId> SqliteChatPersistenceAdapter::PersistChatMessages(std::span<const api::ChatMessage> messages) {
  if (messages.empty()) return std::nullopt;

  auto& db = store_.Database();
  SQLite::Transaction transaction(db);
  SQLite::Statement statement(db, "INSERT INTO chat_messages (id, channel_id, user_id, content) VALUES (?,?,?,?)");

  std::optional<api::PersistenceId> last_persisted_id{std::nullopt};
  for(const auto& message : messages) {
    last_persisted_id = message.id;
    statement.bind(1, message.id);
    statement.bind(2, message.channel_id);
    statement.bind(3, message.user_id);
    statement.bind(4, message.content);

    statement.exec();
    statement.reset();
    statement.clearBindings();
  }

  transaction.commit();

  return last_persisted_id;
}

std::optional<api::PersistenceId>
SqliteChatPersistenceAdapter::GetFirstChatMessageId(api::PersistenceId channel_id) {
  const auto& db = store_.Database();
  SQLite::Statement query(db,
    "SELECT MIN(id) FROM chat_messages "
    "WHERE channel_id = ? "
  );

  query.bind(1, channel_id);
  query.executeStep();

  if (query.isColumnNull(0)) return std::nullopt;
  return query.getColumn(0).getUInt();
}

std::optional<api::PersistenceId>
SqliteChatPersistenceAdapter::GetLastChatMessageId() {
  const auto& db = store_.Database();
  SQLite::Statement query(db,
    "SELECT MAX(id) FROM chat_messages; "
  );

  query.executeStep();

  if (query.isColumnNull(0)) return std::nullopt;
  return query.getColumn(0).getUInt();
}

std::vector<api::ChatMessage>
SqliteChatPersistenceAdapter::GetChatMessagesBefore
(api::PersistenceId channel_id, api::PersistenceId max_message_id, std::uint32_t limit) {
  std::vector<api::ChatMessage> result;
  const auto& db = store_.Database();

  SQLite::Statement query(db,
  R"(SELECT id, user_id, content
    FROM (
      SELECT id, user_id, content
      FROM chat_messages
      WHERE channel_id = ?
        AND id <= ?
      ORDER BY id DESC
      LIMIT ?
    )
    ORDER BY id ASC;
  )");

  query.bind(1, channel_id);
  query.bind(2, max_message_id);
  query.bind(3, limit);

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
    const auto& name = query.getColumn(0).getString();
    return api::ChatChannel{id, name};
  }

  return std::nullopt;
}

std::vector<api::ChatChannel> SqliteChatPersistenceAdapter::GetChatChannels() {
  std::vector<api::ChatChannel> result;
  const auto& db = store_.Database();
  SQLite::Statement query(db,
    "SELECT id, name FROM chat_channels;"
  );

  while (query.executeStep()) {
    auto id = query.getColumn(0).getUInt();
    const auto& name = query.getColumn(1).getString();
    result.emplace_back(id, name);
  }

  return result;
}

}