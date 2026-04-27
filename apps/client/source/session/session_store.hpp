#pragma once

#include "../application/client_rpc_system.hpp"

#include <ebroschin/chat/api.hpp>

#include <span>
#include <functional>
#include <optional>
#include <unordered_map>

namespace ebroschin::chatcore::client {

class SessionSystem;

class SessionStore {
public:
  using UsersView = const std::unordered_map<api::PersistenceId, api::User>&;
  explicit SessionStore(SessionSystem& session_system, ClientRpcSystem& rpc_system) noexcept;

  api::ChatChannel& CacheChannel(const api::ChatChannel& channel);
  api::User& CacheUser(const api::User& user);

  void LoadUsers(network::ConnectionId connection_id,
    std::span<const api::PersistenceId> user_ids,
    std::function<void(UsersView view)> callback);

  void LoadUser(network::ConnectionId connection_id,
    api::PersistenceId user_id,
    std::function<void(std::optional<std::reference_wrapper<const api::User>> user)> callback);

  void LoadChannel(network::ConnectionId connection_id,
    api::PersistenceId channel_id,
    std::function<void(std::optional<std::reference_wrapper<const api::ChatChannel>> channel)> callback);

private:
  SessionSystem& session_system_;
  ClientRpcSystem& rpc_system_;

  std::unordered_map<api::PersistenceId, api::User> users_cache_{};
  std::unordered_map<api::PersistenceId, api::ChatChannel> channels_cache_{};
};

}