#pragma once

#include "../application/commons.h"

#include <claw/chat/api.h>
#include <claw/core/system.h>
#include <optional>
#include <string>
#include <unordered_map>

namespace claw::chat::server {

class UserPersistenceAdapter;

class UserServerSystem final : public core::System {
public:
  explicit UserServerSystem(const core::SystemContext& ctx);

  void Initialize() override;

  std::optional<api::PersistenceId> CreateUser(const std::string& name, const std::string& password);
  bool ValidateSession(std::uint64_t request_type_id, const network::ConnectionId& id);
  std::optional<std::reference_wrapper<const api::User>> GetSessionUser(const network::ConnectionId& id);

private:
  UserPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;

  std::unordered_map<network::ConnectionId, api::User> user_sessions_{};
};

}