#pragma once

#include "../application/chat_tcp_system.h"

#include <claw/chat/api.h>
#include <claw/core/system.h>
#include <optional>
#include <string>

#include "../application/application_system.h"
#include "user_store.h"

namespace claw::chat::server {

class UserPersistenceAdapter;
class ChatServerSystem;

class UserServerSystem final : public core::System {
public:
  explicit UserServerSystem(const core::SystemContext& ctx);

  void Initialize() override;

  [[nodiscard]] bool ValidateSession(network::RequestId, network::ConnectionId) const;
  [[nodiscard]] bool ValidateSession(network::ConnectionId) const;
  void RemoveSession(network::ConnectionId);
  std::optional<std::reference_wrapper<const api::User>> GetSessionUser(network::ConnectionId);

private:
  void HandleCreateUser(network::ConnectionId, const api::CreateUserRequestMessage&);
  void HandleAuthenticateUser(network::ConnectionId, const api::AuthenticateUserRequestMessage&);
  void HandleGetUsers(network::ConnectionId, const api::GetUsersRequestMessage&);

  ApplicationSystem& app_system_;
  UserPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;

  UserStore user_store_{adapter_};
};

}