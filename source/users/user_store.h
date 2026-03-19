#pragma once

#include <claw/chat/api.h>
#include <claw/network/commons.h>

#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/bimap.hpp>

namespace claw::chat::server {

using namespace boost;

class UserPersistenceAdapter;

class UserStore {
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
  struct UserSession {
    network::ConnectionId connection_id{};
    api::User user{};

    [[nodiscard]] const std::string& GetUserName() const { return user.name; }
  };

  using UserMultiIndex = multi_index_container<
    UserSession,
    multi_index::indexed_by<
      multi_index::ordered_unique<
        multi_index::member<UserSession, network::ConnectionId, &UserSession::connection_id>
      >,
      multi_index::ordered_non_unique<
        multi_index::const_mem_fun<UserSession, const std::string&, &UserSession::GetUserName>
      >
    >
  >;

  UserPersistenceAdapter& adapter_;

  UserMultiIndex users_{};
  bimap<network::ConnectionId, api::PersistenceId> user_sessions_{};
};

}



