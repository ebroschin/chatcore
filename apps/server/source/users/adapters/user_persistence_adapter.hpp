#pragma once

#include <ebroschin/chat/api.hpp>
#include <optional>
#include <string>

namespace ebroschin::chatcore::api {
struct User;
}

namespace ebroschin::chatcore::server {

class UserPersistenceAdapter {
public:
  virtual ~UserPersistenceAdapter() = default;

  virtual std::optional<api::User> CreateUser(const std::string& name, const std::string& password) = 0;
  virtual std::optional<api::User> GetUser(api::PersistenceId id) = 0;
  virtual std::optional<api::User> GetUser(const std::string& name) = 0;
  virtual std::vector<api::User> GetUsers() = 0;
  virtual std::optional<api::User> MatchUserCredentials(const std::string& name, const std::string& password) = 0;
};

}