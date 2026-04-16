#include "chat_channel_store.h"

#include "adapters/chat_persistence_adapter.h"

namespace ebroschin::chatcore::server {

ChatChannelStore::ChatChannelStore(ChatPersistenceAdapter& adapter):
  adapter_(adapter)
{}

void ChatChannelStore::Prewarm() {
  auto channels = adapter_.GetChatChannels();
  for (auto& channel : channels) {
    CacheChannel(std::move(channel));
  }
}

std::vector<api::ChatChannel> ChatChannelStore::GetChannels() const {
  const auto view = channels_ | std::views::transform([](const auto& c) { return c.channel; });
  return {view.begin(), view.end()};
}

const api::ChatChannel& ChatChannelStore::CacheChannel(api::ChatChannel channel) {
  auto [it, _] = channels_.emplace(std::move(channel));
  return it->channel;
}

void ChatChannelStore::AssignConnection(network::ConnectionId connection_id, api::PersistenceId channel_id) {
  UnassignConnection(connection_id);
  connection_channel_map_.insert({connection_id, channel_id});
}

void ChatChannelStore::UnassignConnection(network::ConnectionId connection_id) {
  connection_channel_map_.left.erase(connection_id);
}

std::optional<ChatChannelStore::ConnectionsRange> ChatChannelStore::GetConnections(api::PersistenceId channel_id) const {
  const auto& map = connection_channel_map_.right;
  const auto range = map.equal_range(channel_id);
  if (range.first == range.second) return std::nullopt;

  const auto begin = ConnectionsRange::Iterator(range.first, &ConnectionsRange::Transform);
  const auto end = ConnectionsRange::Iterator(range.second, &ConnectionsRange::Transform);
  return ConnectionsRange{begin, end};
}

std::optional<std::reference_wrapper<const api::ChatChannel>>
ChatChannelStore::GetChannel(api::PersistenceId channel_id) {
  const auto& map = channels_.get<0>();
  const auto it = map.find(channel_id);
  if (it != map.end()) return it->channel;

  auto potential_channel = adapter_.GetChatChannel(channel_id);
  if (!potential_channel) return std::nullopt;

  return CacheChannel(std::move(*potential_channel));
}

std::optional<std::reference_wrapper<const api::ChatChannel>>
ChatChannelStore::GetChannel(const std::string& channel_name) {
  const auto& map = channels_.get<1>();
  const auto it = map.find(channel_name);
  if (it != map.end()) return it->channel;

  auto potential_channel = adapter_.GetChatChannel(channel_name);
  if (!potential_channel) return std::nullopt;

  return CacheChannel(std::move(*potential_channel));
}

std::optional<std::reference_wrapper<const api::ChatChannel>> ChatChannelStore::GetAssignedChannel(network::ConnectionId connection_id) {
  const auto& map = connection_channel_map_.left;
  const auto it = map.find(connection_id);
  if (it == map.end()) return std::nullopt;

  return GetChannel(it->second);
}

}