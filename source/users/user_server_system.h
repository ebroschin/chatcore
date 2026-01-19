#pragma once

#include "../application/chat_tcp_system.h"

#include <claw/core/system.h>
#include <string>
#include <unordered_map>
#include <optional>

#include "../application/commons.h"

namespace claw::chat::server {

class UserPersistenceAdapter;

class UserServerSystem final : public core::System {
public:
  explicit UserServerSystem(const core::SystemContext& ctx);

  api::PersistenceID CreateUser(const std::string& name, const std::string& password);
  bool ValidateSession(const network::ConnectionId& id);
  std::optional<std::reference_wrapper<const api::User>> GetSessionUser(const network::ConnectionId& id);

private:
  UserPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;

  std::unordered_map<network::ConnectionId, api::User> user_sessions_{};
};

}