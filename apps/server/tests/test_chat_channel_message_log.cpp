#include <gtest/gtest.h>

#include "../source/chat/chat_channel_message_log.hpp"
#include "../source/chat/chat_message_store.hpp"
#include "mock_chat_persistence_adapter.hpp"

namespace ebroschin::chatcore::server::tests {

TEST(ChatChannelMessageLogTests, AssignMessageIdsUniqueSorted) {
  MockChatPersistenceAdapter adapter{};
  ChatMessageStore store{adapter};

  constexpr api::PersistenceId channel_id = 1;
  ChatChannelMessageLog log{channel_id, store, adapter};
  log.AssignMessageIds({4, 5, 2, 3, 3, 1, 2, 4, 4, 4, 77, 123});

  const auto ids = log.GetLatestChatMessages(100);
  ASSERT_EQ(ids.size(), 7);
  EXPECT_EQ(ids, (std::vector<api::PersistenceId>{1, 2, 3, 4, 5, 77, 123}));
}

}

