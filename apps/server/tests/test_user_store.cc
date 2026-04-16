#include <gtest/gtest.h>

#include "mock_user_persistence_adapter.hpp"
#include "../source/users/user_store.h"

namespace claw::chat::server::tests {

TEST(UserStoreTests, Prewarm) {
  MockUserPersistenceAdapter adapter;
  adapter.CreateUser("dante", "123");
  adapter.CreateUser("vergil", "123");

  UserStore store(adapter);
  store.Prewarm();

  auto test_user = [](UserStore& store, api::PersistenceId id, const std::string& name) {
    const auto user = store.GetUser(name);
    ASSERT_TRUE(user.has_value());
    EXPECT_EQ(user->get().id, id);
    EXPECT_EQ(user->get().name, name);
  };

  test_user(store, 1, "dante");
  test_user(store, 2, "vergil");
}

TEST(UserStoreTests, SessionLifecycle) {
  MockUserPersistenceAdapter adapter;
  adapter.CreateUser("dante", "123");
  adapter.CreateUser("vergil", "123");

  UserStore store{adapter};
  store.Prewarm();

  const auto potential_user1 = store.GetUser(1);
  const auto potential_user2 = store.GetUser(2);
  ASSERT_TRUE(potential_user1.has_value());
  ASSERT_TRUE(potential_user2.has_value());

  const auto& user1 = potential_user1->get();
  const auto& user2 = potential_user2->get();
  EXPECT_EQ(user1.id, 1);
  EXPECT_EQ(user1.name, "dante");
  EXPECT_EQ(user2.id, 2);
  EXPECT_EQ(user2.name, "vergil");

  auto test_user = [](UserStore& store,
    network::ConnectionId connection_id,
    const api::User& user,
    const std::string& expected_name)
  {
    store.AssignSession(connection_id, user.id);
    EXPECT_TRUE(store.HasSession(connection_id));
    EXPECT_TRUE(store.HasSession(user.id));

    const auto session_user = store.GetSessionUser(connection_id);
    ASSERT_TRUE(session_user.has_value());
    EXPECT_EQ(session_user->get().name, expected_name);

    store.RemoveSession(connection_id);
    EXPECT_FALSE(store.HasSession(connection_id));
    EXPECT_FALSE(store.GetSessionUser(connection_id).has_value());
  };

  test_user(store, 99, user1, "dante");
  test_user(store, 100, user2, "vergil");
}

TEST(UserStoreTests, GetUsersSkipMissing) {
  MockUserPersistenceAdapter adapter;
  adapter.CreateUser("dante", "123");
  adapter.CreateUser("vergil", "123");
  adapter.CreateUser("nero", "777");

  UserStore store{adapter};
  store.Prewarm();

  const std::vector<api::PersistenceId> ids{1, 999, 888, 1337, 2};
  const auto users = store.GetUsers(ids);
  ASSERT_EQ(users.size(), 2);
  EXPECT_EQ(users[0].id, 1);
  EXPECT_EQ(users[1].id, 2);
}

}