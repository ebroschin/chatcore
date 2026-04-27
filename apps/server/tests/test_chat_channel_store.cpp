#include <gtest/gtest.h>

#include "../source/chat/chat_channel_store.hpp"
#include "mock_chat_persistence_adapter.hpp"

namespace ebroschin::chatcore::server::tests {

TEST(ChatChannelStoreTests, ConnectionToChannelAssignment) {
  MockChatPersistenceAdapter adapter;
  const auto channel1 = *adapter.CreateChatChannel("channel1");
  const auto channel2 = *adapter.CreateChatChannel("channel2");

  ChatChannelStore store(adapter);
  store.Prewarm();

  constexpr network::ConnectionId connection_id = 7;
  store.AssignConnection(connection_id, channel1.id);

  auto assigned_channel = store.GetAssignedChannel(connection_id);
  ASSERT_TRUE(assigned_channel.has_value());
  EXPECT_EQ(assigned_channel->get().id, 1);

  store.AssignConnection(connection_id, channel2.id);
  assigned_channel = store.GetAssignedChannel(connection_id);
  ASSERT_TRUE(assigned_channel.has_value());
  EXPECT_EQ(assigned_channel->get().id, 2);
}

TEST(ChatChannelStoreTests, GetConnectionsForChannel) {
  MockChatPersistenceAdapter adapter;
  const auto channel1 = *adapter.CreateChatChannel("channel1");
  const auto channel2 = *adapter.CreateChatChannel("channel2");

  ChatChannelStore store(adapter);
  store.Prewarm();

  constexpr network::ConnectionId connection_id1 = 7;
  constexpr network::ConnectionId connection_id2 = 8;
  constexpr network::ConnectionId connection_id3 = 9;
  constexpr network::ConnectionId connection_id4 = 10;

  store.AssignConnection(connection_id1, channel2.id);
  store.AssignConnection(connection_id2, channel2.id);
  store.AssignConnection(connection_id3, channel2.id);
  store.AssignConnection(connection_id4, channel1.id);

  auto range_size = []<typename TRange>(TRange&& range) {
    std::size_t result = 0;
    for (const auto& _ : range) {
      result ++;
    }

    return result;
  };

  auto connections_range = store.GetConnections(channel1.id);
  ASSERT_TRUE(connections_range.has_value());
  EXPECT_EQ(range_size(*connections_range), 1);

  connections_range = store.GetConnections(channel2.id);
  ASSERT_TRUE(connections_range.has_value());
  EXPECT_EQ(range_size(*connections_range), 3);
}

}

