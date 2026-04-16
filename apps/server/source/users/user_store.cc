#include "user_store.h"

#include "adapters/user_persistence_adapter.h"

namespace ebroschin::chatcore::server {

UserStore::UserStore(UserPersistenceAdapter& adapter) noexcept:
  adapter_{adapter}
{}

void UserStore::Prewarm() {
  for (auto& user : adapter_.GetUsers()) {
    users_.emplace(std::move(user));
  }
}

const api::User& UserStore::CacheUser(api::User user) {
  auto [result, _] = users_.emplace(std::move(user));
  return result->user;
}

bool UserStore::HasSession(network::ConnectionId connection_id) const {
  return user_sessions_.left.find(connection_id) != user_sessions_.left.end();
}

bool UserStore::HasSession(api::PersistenceId user_id) const {
  return user_sessions_.right.find(user_id) != user_sessions_.right.end();
}

void UserStore::AssignSession(network::ConnectionId connection_id, api::PersistenceId user_id) {
  user_sessions_.insert({connection_id, user_id});
}

void UserStore::RemoveSession(network::ConnectionId connection_id) {
  user_sessions_.left.erase(connection_id);
}

std::optional<std::reference_wrapper<const api::User>>
UserStore::GetSessionUser(network::ConnectionId connection_id) {
  const auto& map = user_sessions_.left;
  const auto it = map.find(connection_id);
  if (it == map.end()) return std::nullopt;

  return GetUser(it->second);
}

std::optional<std::reference_wrapper<const api::User>>
UserStore::GetUser(api::PersistenceId user_id) {
  auto& map = users_.get<0>();
  const auto it = map.find(user_id);
  if (it != map.end()) return it->user;

  const auto potential_user = adapter_.GetUser(user_id);
  if (!potential_user) return std::nullopt;

  return CacheUser(*potential_user);
}

std::optional<std::reference_wrapper<const api::User>>
UserStore::GetUser(const std::string& name) {
  auto& map = users_.get<1>();
  const auto it = map.find(name);
  if (it != map.end()) return it->user;

  const auto potential_user = adapter_.GetUser(name);
  if (!potential_user) return std::nullopt;

  return CacheUser(*potential_user);
}

std::vector<api::User> UserStore::GetUsers(std::span<const api::PersistenceId> user_ids) {
  std::vector<api::User> result;
  result.reserve(user_ids.size());

  for (const auto& user_id : user_ids) {
    const auto potential_user = GetUser(user_id);
    if (!potential_user) continue;
    result.emplace_back(*potential_user);
  }

  return result;
}

}