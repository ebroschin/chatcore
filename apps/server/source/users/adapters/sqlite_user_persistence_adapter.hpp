#pragma once

#include "user_persistence_adapter.hpp"

#include <ebroschin/persistence/persistence_adapter.hpp>
#include <ebroschin/persistence-modules/sqlite/sqlite_persistence_store.hpp>

using namespace ebroschin::persistence::modules::sqlite;

namespace ebroschin::chat::api {
struct User;
}

namespace ebroschin::chatcore::server {

class SqliteUserPersistenceAdapter final : public persistence::PersistenceAdapter<SqlitePersistenceStore, UserPersistenceAdapter> {
public:
  explicit SqliteUserPersistenceAdapter(SqlitePersistenceStore& store)
  : PersistenceAdapter(store)
  {}

  void Initialize() override;

  std::optional<api::User> CreateUser(const std::string& name, const std::string& password) override;
  std::optional<api::User> GetUser(api::PersistenceId id) override;
  std::optional<api::User> GetUser(const std::string& name) override;
  std::vector<api::User> GetUsers() override;
  std::optional<api::User> MatchUserCredentials(const std::string& name, const std::string& password) override;
};

}