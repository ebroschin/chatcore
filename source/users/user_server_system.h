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

  bool ValidateSession(network::RequestId request_id, const network::ConnectionId& id) const;
  bool ValidateSession(const network::ConnectionId& id) const;
  std::optional<std::reference_wrapper<const api::User>> GetSessionUser(const network::ConnectionId& id);

private:
  UserPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;

  std::unordered_map<network::ConnectionId, api::User> user_sessions_{};
};

}