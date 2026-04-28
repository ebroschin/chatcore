#include <gtest/gtest.h>

#include "../source/chat/chat_message_store.hpp"
#include "mock_chat_persistence_adapter.hpp"

#include <limits>

namespace ebroschin::chatcore::server::tests {

constexpr auto MaxMessageId = std::numeric_limits<api::PersistenceId>::max();

TEST(ChatMessageStoreTests, PrewarmAndPersist) {
  constexpr api::PersistenceId channel_id = 1;
  constexpr api::PersistenceId user_id = 1;
  const std::string channel_name = "ctest-channel";

  MockChatPersistenceAdapter adapter{};

  //Mock persisted channels on application start
  adapter.CreateChatChannel(channel_name);

  //Mock persisted messages on application start
  adapter.CreateChatMessage(channel_id, user_id, "a");
  adapter.CreateChatMessage(channel_id, user_id, "b");
  adapter.CreateChatMessage(channel_id, user_id, "c");

  ChatMessageStore store{adapter};
  store.Prewarm();

  //The cache acts as the authoritative source of truth after prewarm
  store.CreateMessage(channel_id, user_id, "d");

  const auto messages = store.GetMessagesBefore(1, MaxMessageId, 10);
  ASSERT_GE(messages.size(), 1u);

  const auto& last_message = messages.back();
  EXPECT_EQ(last_message.id, 4u);
  EXPECT_EQ(last_message.content, "d");

  //Before Persist() call: message with id 4 is not yet persisted
  auto last_persisted_message_id = adapter.GetLastChatMessageId();
  EXPECT_EQ(last_persisted_message_id, 3u);

  store.Persist();

  //After Persist() call: message with id 4 is now persisted
  last_persisted_message_id = adapter.GetLastChatMessageId();
  EXPECT_EQ(last_persisted_message_id, 4u);
}

TEST(ChatMessageStoreTests, MultiChannelPrewarm) {
  constexpr api::PersistenceId user_id = 1;
  constexpr std::size_t initial_channel1_message_count = 5;
  constexpr std::size_t initial_channel2_message_count = 7;
  constexpr std::size_t initial_channel3_message_count = 11;

  MockChatPersistenceAdapter adapter{};
  //Mock persisted channels on application start
  const auto channel1 = *adapter.CreateChatChannel("ctest-enjoyers1");
  const auto channel2 = *adapter.CreateChatChannel("ctest-enjoyers2");
  const auto channel3 = *adapter.CreateChatChannel("ctest-enjoyers3");

  //Mock persisted messages on application start
  for (std::size_t i = 0; i < initial_channel1_message_count; i++) {
    adapter.CreateChatMessage(channel1.id, user_id, "a" + std::to_string(i));
  }

  for (std::size_t i = 0; i < initial_channel2_message_count; i++) {
    adapter.CreateChatMessage(channel2.id, user_id, "b" + std::to_string(i));
  }

  for (std::size_t i = 0; i < initial_channel3_message_count; i++) {
    adapter.CreateChatMessage(channel3.id, user_id, "c" + std::to_string(i));
  }

  ChatMessageStore store{adapter};

  //Prior to Prewarm(), no messages exist within the cache
  auto messages = store.GetMessagesBefore(channel1.id, MaxMessageId, 20);
  ASSERT_EQ(messages.size(), 0);

  messages = store.GetMessagesBefore(channel2.id, MaxMessageId, 20);
  ASSERT_EQ(messages.size(), 0);

  messages = store.GetMessagesBefore(channel3.id, MaxMessageId, 20);
  ASSERT_EQ(messages.size(), 0);

  //After Prewarm(), persisted messages are cached
  store.Prewarm();

  messages = store.GetMessagesBefore(channel1.id, MaxMessageId, 20);
  ASSERT_EQ(messages.size(), initial_channel1_message_count);

  messages = store.GetMessagesBefore(channel2.id, MaxMessageId, 20);
  ASSERT_EQ(messages.size(), initial_channel2_message_count);

  messages = store.GetMessagesBefore(channel3.id, MaxMessageId, 20);
  ASSERT_EQ(messages.size(), initial_channel3_message_count);
}

TEST(ChatMessageStoreTests, Paging) {
  constexpr api::PersistenceId user_id = 1;
  constexpr std::size_t initial_channel_message_count = 55;

  MockChatPersistenceAdapter adapter{};
  //Mock persisted channels on application start
  const auto channel = *adapter.CreateChatChannel("ctest-enjoyers");

  //Mock persisted messages on application start
  for (std::size_t i = 0; i < initial_channel_message_count; i++) {
    adapter.CreateChatMessage(channel.id, user_id, "a" + std::to_string(i));
  }

  ChatMessageStore store{adapter};
  store.Prewarm();

  //Retrieve the latest 5 messages
  auto messages = store.GetMessagesBefore(channel.id, MaxMessageId, 5);
  ASSERT_EQ(messages.size(), 5);

  //Retrieve 5 messages, starting with the message at id 3
  messages = store.GetMessagesBefore(channel.id, 3, 5);
  ASSERT_EQ(messages.size(), 2);

  //Insert 2 messages, get 5 latest again
  store.CreateMessage(user_id, channel.id, "test");
  store.CreateMessage(user_id, channel.id, "test");

  messages = store.GetMessagesBefore(channel.id, MaxMessageId, 5);
  ASSERT_EQ(messages.size(), 5);

  //Retrieve the last 100 messages (more than initial messages + recently added messages)
  messages = store.GetMessagesBefore(channel.id, MaxMessageId, 100);
  ASSERT_EQ(messages.size(), initial_channel_message_count + 2);
}

}