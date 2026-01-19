#pragma once

#include "../../persistence/persistence_adapter.h"
#include "../../application/commons.h"
#include <string>
#include <optional>

namespace claw::chat::api {
struct User;
}

namespace claw::chat::server {

class UserPersistenceAdapter : public virtual persistence::PersistenceAdapterBase {
public:
  virtual api::PersistenceID CreateUser(const std::string& name, const std::string& password) = 0;
  virtual std::optional<api::User> GetUser(const api::PersistenceID& id) = 0;
  virtual std::optional<api::User> AuthenticateUser(const std::string& name, const std::string& password) = 0;
};

}