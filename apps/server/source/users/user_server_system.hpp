#pragma once

#include "../application/chat_tcp_system.hpp"
#include "user_store.hpp"

#include <ebroschin/chat/api.hpp>
#include <ebroschin/core/system.hpp>

#include <optional>

namespace ebroschin::chatcore::server {

class UserPersistenceAdapter;
class ChatServerSystem;
class ChatServerApplication;

class UserServerSystem final : public core::System {
public:
  explicit UserServerSystem(const core::SystemContext& ctx, ChatServerApplication& app) noexcept;

  void Initialize() override;

  void RemoveSession(network::ConnectionId);

  [[nodiscard]] bool ValidateSession(network::RequestId, network::ConnectionId) const;
  [[nodiscard]] bool ValidateSession(network::ConnectionId) const;
  [[nodiscard]] std::optional<std::reference_wrapper<const api::User>> GetSessionUser(network::ConnectionId);

private:
  void HandleCreateUser(network::ConnectionId, const api::CreateUserRequestMessage&);
  void HandleAuthenticateUser(network::ConnectionId, const api::AuthenticateUserRequestMessage&);
  void HandleGetUsers(network::ConnectionId, const api::GetUsersRequestMessage&);
  void HandleGetUser(network::ConnectionId, const api::GetUserRequestMessage&);

  ChatServerApplication& app_;
  UserPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;

  UserStore user_store_{adapter_};
};

}