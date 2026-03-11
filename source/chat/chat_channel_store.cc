#include "chat_channel_store.h"

#include "adapters/chat_persistence_adapter.h"

namespace claw::chat::server {

ChatChannelStore::ChatChannelStore(ChatPersistenceAdapter& adapter):
  adapter_(adapter)
{}

void ChatChannelStore::Prewarm() {
  auto channels = adapter_.GetChatChannels();
  for (auto& channel : channels) {
    CacheChannel(std::move(channel));
  }
}

api::ChatChannel& ChatChannelStore::CacheChannel(api::ChatChannel channel) {
  const auto channel_id = channel.id;
  auto [it, _] = channel_cache_.emplace(channel_id, std::move(channel));
  return it->second;
}

void ChatChannelStore::AssignConnection(network::ConnectionId connection_id, api::PersistenceId channel_id) {
  UnassignConnection(connection_id);
  channel_to_connections_lookup_[channel_id].emplace(connection_id);
  connection_to_channel_lookup_[connection_id] = channel_id;
}

void ChatChannelStore::UnassignConnection(network::ConnectionId connection_id) {
  const auto connection_to_channel_iterator = connection_to_channel_lookup_.find(connection_id);
  if (connection_to_channel_iterator == connection_to_channel_lookup_.end()) return;

  const auto channel_id = connection_to_channel_iterator->second;
  const auto channel_to_connections_iterator = channel_to_connections_lookup_.find(channel_id);
  if (channel_to_connections_iterator == channel_to_connections_lookup_.end()) return;

  auto& connections_set = channel_to_connections_iterator->second;
  connections_set.erase(connection_id);
  connection_to_channel_lookup_.erase(connection_to_channel_iterator);
}

std::optional<ChatChannelStore::ConnectionsRange> ChatChannelStore::GetConnections(api::PersistenceId channel_id) {
  const auto it = channel_to_connections_lookup_.find(channel_id);
  if (it == channel_to_connections_lookup_.end()) return std::nullopt;

  const auto& set = it->second;
  return ConnectionsRange{set.cbegin(), set.cend()};
}

std::optional<std::reference_wrapper<const api::ChatChannel>>
ChatChannelStore::GetChannel(api::PersistenceId channel_id) {
  const auto* cached_channel = GetCachedChannel(channel_id);
  if (cached_channel != nullptr) return *cached_channel;

  auto potential_channel = adapter_.GetChatChannel(channel_id);
  if (!potential_channel) return std::nullopt;

  return CacheChannel(std::move(*potential_channel));
}

std::optional<std::reference_wrapper<const api::ChatChannel>>
ChatChannelStore::GetChannel(const std::string& channel_name) {
  for (const auto& channel : channel_cache_ | std::ranges::views::values) {
    if (channel.name != channel_name) continue;
    return channel;
  }

  auto potential_channel = adapter_.GetChatChannel(channel_name);
  if (!potential_channel) return std::nullopt;

  return CacheChannel(std::move(*potential_channel));
}

std::optional<std::reference_wrapper<const api::ChatChannel>> ChatChannelStore::GetAssignedChannel(network::ConnectionId connection_id) {
  const auto it = connection_to_channel_lookup_.find(connection_id);
  if (it == connection_to_channel_lookup_.end()) return std::nullopt;

  const auto id = it->second;
  return GetChannel(id);
}

api::ChatChannel* ChatChannelStore::GetCachedChannel(api::PersistenceId channel_id) {
  const auto it = channel_cache_.find(channel_id);
  if (it == channel_cache_.end()) return nullptr;

  return &it->second;
}

}