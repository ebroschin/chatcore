#pragma once

#include <claw/chat/api.h>
#include <claw/network/commons.h>

#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/bimap.hpp>

namespace claw::chat::server {

using namespace boost;

class UserPersistenceAdapter;

class UserStore {
  struct CachedUser {
    api::User user{};

    [[nodiscard]] const api::PersistenceId& GetUserId() const { return user.id; }
    [[nodiscard]] const std::string& GetUserName() const { return user.name; }
  };

  using UserMultiIndex = multi_index_container<
    CachedUser,
    multi_index::indexed_by<
      multi_index::ordered_unique<
        multi_index::const_mem_fun<CachedUser, const api::PersistenceId&, &CachedUser::GetUserId>
      >,
      multi_index::ordered_non_unique<
        multi_index::const_mem_fun<CachedUser, const std::string&, &CachedUser::GetUserName>
      >
    >
  >;

public:
  explicit UserStore(UserPersistenceAdapter& adapter) noexcept;

  void Prewarm();
  const api::User& CacheUser(api::User user);
  bool HasSession(network::ConnectionId connection_id) const;
  bool HasSession(api::PersistenceId user_id) const;
  void AssignSession(network::ConnectionId connection_id, api::PersistenceId user_id);
  std::optional<std::reference_wrapper<const api::User>> GetSessionUser(network::ConnectionId connection_id);
  void RemoveSession(network::ConnectionId connection_id);

  std::optional<std::reference_wrapper<const api::User>> GetUser(api::PersistenceId user_id);
  std::optional<std::reference_wrapper<const api::User>> GetUser(const std::string& name);
  std::vector<api::User> GetUsers(std::span<const api::PersistenceId> user_ids);

private:
  UserPersistenceAdapter& adapter_;

  UserMultiIndex users_{};
  bimap<network::ConnectionId, api::PersistenceId> user_sessions_{};
};

}



