#pragma once

#include <claw/chat/api.h>
#include <claw/network/commons.h>
#include <optional>
#include <ranges>

#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/iterator/transform_iterator.hpp>

namespace claw::chat::server {

using namespace boost;

class ChatPersistenceAdapter;
struct ConnectionsRange;

class ChatChannelStore {
  struct CachedChannel {
    api::ChatChannel channel{};

    [[nodiscard]] const api::PersistenceId& GetId() const { return channel.id; }
    [[nodiscard]] const std::string& GetName() const { return channel.name; }
  };

  using ChannelMultiIndex = multi_index_container<
    CachedChannel,
    multi_index::indexed_by<
      multi_index::ordered_unique<
        multi_index::const_mem_fun<CachedChannel, const api::PersistenceId&, &CachedChannel::GetId>
      >,
      multi_index::ordered_non_unique<
        multi_index::const_mem_fun<CachedChannel, const std::string&, &CachedChannel::GetName>
      >
    >
  >;

  using ConnectionChannelMap = bimap<
    bimaps::unordered_set_of<network::ConnectionId>,
    bimaps::unordered_multiset_of<api::PersistenceId>
  >;

  struct ConnectionsRange {
    using PairIterator = ConnectionChannelMap::right_map::const_iterator;
    using Pair = ConnectionChannelMap::right_map::value_type;
    using ValueType = Pair::second_type;

    static ValueType Transform(const Pair& value) {
      return value.second;
    }

    using Iterator = transform_iterator<decltype(&Transform), PairIterator>;

    Iterator begin_iterator;
    Iterator end_iterator;

    [[nodiscard]] Iterator begin() const noexcept { return begin_iterator; }
    [[nodiscard]] Iterator end() const noexcept { return end_iterator; }
  };

public:
  explicit ChatChannelStore(ChatPersistenceAdapter& adapter);

  void Prewarm();
  const api::ChatChannel& CacheChannel(api::ChatChannel channel);

  void AssignConnection(network::ConnectionId connection_id, api::PersistenceId channel_id);
  void UnassignConnection(network::ConnectionId connection_id);
  std::optional<std::reference_wrapper<const api::ChatChannel>> GetAssignedChannel(network::ConnectionId connection_id);

  [[nodiscard]] std::optional<ConnectionsRange> GetConnections(api::PersistenceId channel_id) const;
  std::optional<std::reference_wrapper<const api::ChatChannel>> GetChannel(api::PersistenceId channel_id);
  std::optional<std::reference_wrapper<const api::ChatChannel>> GetChannel(const std::string& channel_name);

  [[nodiscard]] std::vector<api::ChatChannel> GetChannels() const;

private:
  ChatPersistenceAdapter& adapter_;

  ChannelMultiIndex channels_{};
  ConnectionChannelMap connection_channel_map_{};
};

}