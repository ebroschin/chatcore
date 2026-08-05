#include "session_store.hpp"

#include "session_system.hpp"
#include "../application/chat_client_application.hpp"

namespace ebroschin::chatcore::client {

SessionStore::SessionStore(ClientRpcSystem& rpc_system, ChatClientApplication& app) noexcept:
  rpc_system_{rpc_system},
  app_{app}
{}

const api::User& SessionStore::CacheUser(const api::User& user) {
  const auto key = user.id;
  const auto [it, _] = users_cache_.try_emplace(key, user);
  return it->second;
}

const api::ChatChannel& SessionStore::CacheChannel(const api::ChatChannel& channel) {
  const auto channel_id = channel.id;
  const auto [it, _] = channels_cache_.try_emplace(channel_id, channel);
  return it->second;
}

void SessionStore::LoadUsers(network::ConnectionId connection_id,
  std::span<const api::PersistenceId> user_ids,
  std::function<void(UsersView view)> callback)
{
  std::vector<api::PersistenceId> uncached_ids{};
  for (const auto& user_id : user_ids) {
    const auto it = users_cache_.find(user_id);
    if (it != users_cache_.end()) continue;

    uncached_ids.emplace_back(user_id);
  }

  if (uncached_ids.empty()) {
    if (!callback) return;
    callback(users_cache_);
    return;
  }

  auto get_user_rpc = rpc_system_.Prepare<api::GetUsersRequestMessage>(connection_id, uncached_ids);
  get_user_rpc.OnSuccess([this, callback = std::move(callback)]
  (const api::GetUsersResponseMessage& response) mutable
  {
    for (const auto& user : response.users) {
      const auto id = user.id;
      users_cache_.try_emplace(id, user);
    }

    if (!callback) return;
    callback(users_cache_);
  });

  app_.RegisterDefaultErrorHandler(get_user_rpc);
  app_.RegisterDefaultTimeoutHandler(get_user_rpc, "Timed out requesting user");
  get_user_rpc.Call();
}

void SessionStore::LoadUser(network::ConnectionId connection_id,
  api::PersistenceId user_id,
  std::function<void(std::optional<std::reference_wrapper<const api::User>> user)> callback)
{
  const auto it = users_cache_.find(user_id);
  if (it != users_cache_.end()) {
    if (!callback) return;
    callback(it->second);
    return;
  }

  LoadUsers(connection_id, std::vector{user_id}, [user_id, callback = std::move(callback)]
  (UsersView view)
  {
    if (!callback) return;

    const auto view_it = view.find(user_id);
    if (view_it == view.end()) {
      callback(std::nullopt);
      return;
    }

    callback(view_it->second);
  });
}

void SessionStore::LoadChannel(network::ConnectionId connection_id,
  api::PersistenceId channel_id,
  std::function<void(std::optional<std::reference_wrapper<const api::ChatChannel>> channel)> callback) {
  const auto it = channels_cache_.find(channel_id);
  if (it != channels_cache_.end()) {
    if (!callback) return;
    callback(it->second);
    return;
  }

  auto get_channel_rpc = rpc_system_.Prepare<api::GetChatChannelRequestMessage>(connection_id, channel_id);
  get_channel_rpc.OnSuccess([this, callback = std::move(callback)]
  (const api::GetChatChannelResponseMessage& response)
  {
    const auto& cached_channel = CacheChannel(response.channel);

    if (!callback) return;
    callback(cached_channel);
  });

  app_.RegisterDefaultErrorHandler(get_channel_rpc);
  app_.RegisterDefaultTimeoutHandler(get_channel_rpc, "Timed out requesting channel");
  get_channel_rpc.Call();
}

}