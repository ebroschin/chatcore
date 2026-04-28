#pragma once

#include "../source/users/adapters/user_persistence_adapter.hpp"

#include <ranges>
#include <unordered_map>
#include <vector>

namespace ebroschin::chatcore::server::tests {

struct MockUser {
  api::User user;
  std::string password;
};

class MockUserPersistenceAdapter final : public UserPersistenceAdapter {
public:
  std::optional<api::User> CreateUser(const std::string& name, const std::string& password) override {
    const auto id = next_id++;
    auto [it, _] = mock_users_.try_emplace(id, MockUser{{id, name}, password});
    return it->second.user;
  }

  [[nodiscard]] std::optional<api::User> GetUser(api::PersistenceId id) override {
    const auto it = mock_users_.find(id);
    if (it == mock_users_.end()) return std::nullopt;

    return it->second.user;
  }

  [[nodiscard]] std::optional<api::User> GetUser(const std::string& name) override {
    for (const auto& mock_user : mock_users_ | std::ranges::views::values) {
      if (mock_user.user.name == name) return mock_user.user;
    }

    return std::nullopt;
  }

  [[nodiscard]] std::vector<api::User> GetUsers() override {
    auto view = mock_users_ | std::ranges::views::values | std::views::transform([](const auto& x) { return x.user; });
    return std::vector<api::User>{view.begin(), view.end()};
  }

  std::optional<api::User> MatchUserCredentials(const std::string& name, const std::string& password) override {
    for (const auto& mock_user : mock_users_ | std::ranges::views::values) {
      if (mock_user.user.name != name) continue;
      if (mock_user.password != password) continue;

      return mock_user.user;
    }

    return std::nullopt;
  }

private:
  std::unordered_map<api::PersistenceId, MockUser> mock_users_{};
  api::PersistenceId next_id{1};
};

}