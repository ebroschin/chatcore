#include "sqlite_user_persistence_adapter.h"
#include <claw/chat/api.h>

namespace claw::chat::server {

void SqliteUserPersistenceAdapter::Initialize() {
  auto& db = store_.Database();
  db.exec("PRAGMA foreign_keys = ON;");
  db.exec(R"(
    CREATE TABLE IF NOT EXISTS chat_users (
        id INTEGER PRIMARY KEY,
        name TEXT NOT NULL,
        password TEXT NOT NULL
    );
  )");
}

void SqliteUserPersistenceAdapter::Deinitialize()  {

}

api::PersistenceId SqliteUserPersistenceAdapter::CreateUser(const std::string& name, const std::string& password)  {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_users (name, password) VALUES (?,?);");
  query.bind(1, name);
  query.bind(2, password);
  query.exec();

  return static_cast<api::PersistenceId>(store_.Database().getLastInsertRowid());
}

std::optional<api::User> SqliteUserPersistenceAdapter::GetUser(const api::PersistenceId& id)  {
  const auto& db = store_.Database();
  SQLite::Statement query(db,
      "SELECT id, name FROM chat_users "
      "WHERE id = ? "
      "ORDER BY id ASC;"
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

std::optional<api::User> SqliteUserPersistenceAdapter::AuthenticateUser(const std::string& name, const std::string& password)  {
  const auto& db = store_.Database();
  SQLite::Statement query(db,
      "SELECT id FROM chat_users "
      "WHERE name = ? "
      "AND password = ? "
      "ORDER BY id ASC;"
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


}