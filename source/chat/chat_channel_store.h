#pragma once

#include <claw/chat/api.h>
#include <claw/network/commons.h>
#include <optional>

#include <unordered_map>
#include <unordered_set>

namespace claw::chat::server {

class ChatPersistenceAdapter;

class ChatChannelStore {
public:
  struct ConnectionsRange {
    using Iterator = std::unordered_set<network::ConnectionId>::const_iterator;

    Iterator begin_iterator;
    Iterator end_iterator;

    [[nodiscard]] Iterator begin() const noexcept { return begin_iterator; }
    [[nodiscard]] Iterator end() const noexcept { return end_iterator; }
  };

  explicit ChatChannelStore(ChatPersistenceAdapter& persistence_adapter);

  void AssignConnection(network::ConnectionId connection_id, api::PersistenceId channel_id);
  void UnassignConnection(network::ConnectionId connection_id);
  std::optional<std::reference_wrapper<const api::ChatChannel>> GetAssignedChannel(network::ConnectionId connection_id);

  std::optional<ConnectionsRange> GetConnections(api::PersistenceId channel_id);
  std::optional<std::reference_wrapper<const api::ChatChannel>> GetChannel(api::PersistenceId channel_id);

private:
  api::ChatChannel* GetCachedChannel(api::PersistenceId channel_id);

  ChatPersistenceAdapter& persistence_adapter_;
  std::unordered_map<api::PersistenceId, api::ChatChannel> channel_cache_{};
  std::unordered_map<network::ConnectionId, api::PersistenceId> connection_to_channel_lookup_{};
  std::unordered_map<api::PersistenceId, std::unordered_set<network::ConnectionId>> channel_to_connections_lookup_{};
};

}