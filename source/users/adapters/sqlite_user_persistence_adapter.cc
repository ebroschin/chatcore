#include "sqlite_user_persistence_adapter.h"
#include <claw/chat/api.h>

namespace claw::chat::server {

void SqliteUserPersistenceAdapter::Initialize() {
  auto& db = store_.Database();
  db.exec("PRAGMA foreign_keys = ON;");
  db.exec(R"(
    CREATE TABLE IF NOT EXISTS chat_users (
        id INTEGER PRIMARY KEY,
        name TEXT NOT NULL UNIQUE,
        password TEXT NOT NULL
    );
  )");
}

void SqliteUserPersistenceAdapter::Deinitialize() { }

std::optional<api::PersistenceId> SqliteUserPersistenceAdapter::CreateUser(const std::string& name, const std::string& password) {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_users (name, password) VALUES (?,?) ON CONFLICT(name) DO NOTHING;");
  query.bind(1, name);
  query.bind(2, password);

  if (query.exec() <= 0) return std::nullopt;
  return static_cast<api::PersistenceId>(store_.Database().getLastInsertRowid());
}

std::optional<api::User> SqliteUserPersistenceAdapter::GetUser(const api::PersistenceId& id) {
  const auto& db = store_.Database();
  SQLite::Statement query(db,
      "SELECT id, name FROM chat_users "
      "WHERE id = ? "
      "ORDER BY id ASC "
      "LIMIT 1;"
  );

  query.bind(1, id);

  while (query.executeStep()) {
    auto user_id = query.getColumn(0).getUInt();
    auto name = query.getColumn(1).getString();
    return api::User{user_id, name};
  }

  return std::nullopt;
}

std::optional<api::User> SqliteUserPersistenceAdapter::GetUser(const std::string& name) {
  const auto& db = store_.Database();
  SQLite::Statement query(db,
      "SELECT id FROM chat_users "
      "WHERE name = ? "
      "ORDER BY id ASC "
      "LIMIT 1;"
  );

  query.bind(1, name);

  while (query.executeStep()) {
    auto user_id = query.getColumn(0).getUInt();
    return api::User{user_id, name};
  }

  return std::nullopt;
}

std::optional<api::User> SqliteUserPersistenceAdapter::AuthenticateUser(const std::string& name, const std::string& password) {
  const auto& db = store_.Database();
  SQLite::Statement query(db,
      "SELECT id FROM chat_users "
      "WHERE name = ? "
      "AND password = ? "
      "ORDER BY id ASC "
      "LIMIT 1;"
  );

  query.bind(1, name);
  query.bind(2, password);

  while (query.executeStep()) {
    auto user_id = query.getColumn(0).getUInt();
    return api::User{user_id, name};
  }

  return std::nullopt;
}

std::vector<api::User> SqliteUserPersistenceAdapter::GetUsers(const std::vector<api::PersistenceId>& ids) {
  const auto& db = store_.Database();

  std::vector<api::User> result;
  if (ids.empty()) return result;

  std::ostringstream placeholders;
  placeholders << "?";
  for (std::size_t i = 1; i < ids.size(); i++) {
    placeholders << ", ?";
  }

  SQLite::Statement query(db,
      "SELECT id, name FROM chat_users "
      "WHERE id IN (" + placeholders.str() + ");"
  );

  for (std::size_t i = 0; i < ids.size(); i++) {
    query.bind(static_cast<int>(i + 1), ids[i]);
  }

  while (query.executeStep()) {
    auto user_id = query.getColumn(0).getUInt();
    auto name = query.getColumn(1).getString();
    result.emplace_back(user_id, name);
  }

  return result;
}

}