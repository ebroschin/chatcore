#pragma once

#include "../../persistence/persistence_adapter.h"
#include "../../sqlite_persistence/sqlite_persistence_store.h"
#include "user_persistence_adapter.h"

using namespace claw::persistence::sqlite;

namespace claw::chat::api {
struct User;
}

namespace claw::chat::server {

class SqliteUserPersistenceAdapter final : public persistence::PersistenceAdapter<SqlitePersistenceStore, UserPersistenceAdapter> {
public:
  explicit SqliteUserPersistenceAdapter(SqlitePersistenceStore& store)
  : PersistenceAdapter(store)
  {}

  void Initialize() override;
  void Deinitialize() override;

  std::optional<api::PersistenceId> CreateUser(const std::string& name, const std::string& password) override;
  std::optional<api::User> GetUser(const api::PersistenceId& id) override;
  std::optional<api::User> GetUser(const std::string& name) override;
  std::vector<api::User> GetUsers(const std::vector<api::PersistenceId>& ids) override;
  std::optional<api::User> AuthenticateUser(const std::string& name, const std::string& password) override;
};

}