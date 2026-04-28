#include "sqlite_user_persistence_adapter.hpp"

#include <ebroschin/chat/api.hpp>

namespace ebroschin::chatcore::server {

SqliteUserPersistenceAdapter::SqliteUserPersistenceAdapter(SqlitePersistenceStore& store) noexcept:
  PersistenceAdapter{store}
{}

void SqliteUserPersistenceAdapter::Initialize() {
  auto& db = store_.GetDatabase();
  db.exec(R"(
    CREATE TABLE IF NOT EXISTS chat_users (
        id INTEGER PRIMARY KEY,
        name TEXT NOT NULL UNIQUE,
        password TEXT NOT NULL
    );
  )");
}

std::optional<api::User> SqliteUserPersistenceAdapter::CreateUser(const std::string& name, const std::string& password) {
  const auto& db = store_.GetDatabase();
  SQLite::Statement query{db, "INSERT INTO chat_users (name, password) VALUES (?,?) ON CONFLICT(name) DO NOTHING;"};
  query.bind(1, name);
  query.bind(2, password);

  if (query.exec() <= 0) return std::nullopt;
  const auto user_id = static_cast<api::PersistenceId>(db.getLastInsertRowid());
  return api::User{user_id, name};
}

std::optional<api::User> SqliteUserPersistenceAdapter::GetUser(api::PersistenceId id) {
  const auto& db = store_.GetDatabase();
  SQLite::Statement query{db,
      "SELECT id, name FROM chat_users "
      "WHERE id = ? "
      "ORDER BY id ASC "
      "LIMIT 1;"
  };

  query.bind(1, id);
  if (!query.executeStep()) return std::nullopt;

  const auto user_id = query.getColumn(0).getUInt();
  auto name = query.getColumn(1).getString();
  return api::User{user_id, std::move(name)};
}

std::optional<api::User> SqliteUserPersistenceAdapter::GetUser(const std::string& name) {
  const auto& db = store_.GetDatabase();
  SQLite::Statement query{db,
      "SELECT id FROM chat_users "
      "WHERE name = ? "
      "ORDER BY id ASC "
      "LIMIT 1;"
  };

  query.bind(1, name);
  if (!query.executeStep()) return std::nullopt;

  const auto user_id = query.getColumn(0).getUInt();
  return api::User{user_id, name};
}

std::optional<api::User> SqliteUserPersistenceAdapter::MatchUserCredentials(const std::string& name, const std::string& password) {
  const auto& db = store_.GetDatabase();
  SQLite::Statement query{db,
      "SELECT id FROM chat_users "
      "WHERE name = ? "
      "AND password = ? "
      "ORDER BY id ASC "
      "LIMIT 1;"
  };

  query.bind(1, name);
  query.bind(2, password);
  if (!query.executeStep()) return std::nullopt;

  const auto user_id = query.getColumn(0).getUInt();
  return api::User{user_id, name};
}

std::vector<api::User> SqliteUserPersistenceAdapter::GetUsers() {
  const auto& db = store_.GetDatabase();

  std::vector<api::User> result{};
  SQLite::Statement query{db, "SELECT id, name FROM chat_users;"};

  while (query.executeStep()) {
    auto user_id = query.getColumn(0).getUInt();
    auto name = query.getColumn(1).getString();
    result.emplace_back(user_id, name);
  }

  return result;
}

}