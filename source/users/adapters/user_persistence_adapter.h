#pragma once

#include "../../persistence/persistence_adapter.h"
#include <claw/chat/api.h>
#include <string>
#include <optional>

namespace claw::chat::api {
struct User;
}

namespace claw::chat::server {

class UserPersistenceAdapter {
public:
  virtual ~UserPersistenceAdapter() = default;

  virtual std::optional<api::User> CreateUser(const std::string& name, const std::string& password) = 0;
  virtual std::optional<api::User> GetUser(const api::PersistenceId& id) = 0;
  virtual std::optional<api::User> GetUser(const std::string& name) = 0;
  virtual std::vector<api::User> GetUsers(const std::vector<api::PersistenceId>& ids) = 0;
  virtual std::vector<api::User> GetUsers() = 0;
  virtual std::optional<api::User> MatchUserCredentials(const std::string& name, const std::string& password) = 0;
};

}