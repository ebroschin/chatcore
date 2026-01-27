#pragma once

#include "../application/chat_tcp_system.h"

#include <claw/core/system.h>
#include <string>
#include <unordered_map>
#include <optional>
#include <claw/chat/api.h>

namespace claw::chat::server {

class UserPersistenceAdapter;

class UserServerSystem final : public core::System {
public:
  explicit UserServerSystem(const core::SystemContext& ctx);

  std::optional<api::PersistenceId> CreateUser(const std::string& name, const std::string& password);
  bool ValidateSession(std::uint64_t request_type_id, const network::ConnectionId& id);
  std::optional<std::reference_wrapper<const api::User>> GetSessionUser(const network::ConnectionId& id);

private:
  UserPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;

  //this is state. this will need to be thread safe as soon as multiple message processors perform actions on this object
  std::unordered_map<network::ConnectionId, api::User> user_sessions_{};
};

}