#pragma once

#include "../application/chat_tcp_system.h"

#include <claw/chat/api.h>
#include <claw/core/system.h>
#include <optional>
#include <string>
#include <unordered_map>

#include "../application/application_system.h"

namespace claw::chat::server {

class UserPersistenceAdapter;

class UserServerSystem final : public core::System {
public:
  explicit UserServerSystem(const core::SystemContext& ctx);

  void Initialize() override;

  bool ValidateSession(network::RequestId request_id, network::ConnectionId id) const;
  bool ValidateSession(network::ConnectionId id) const;
  std::optional<std::reference_wrapper<const api::User>> GetSessionUser(network::ConnectionId id);

private:
  void HandleCreateUser(network::ConnectionId, const api::CreateUserRequestMessage&);
  void HandleAuthenticateUser(network::ConnectionId, const api::AuthenticateUserRequestMessage&);
  void HandleGetUsers(network::ConnectionId, const api::GetUsersRequestMessage&);

  //TODO code duplication, write helper utility, mixin or something
  template <typename TMessage>
  void RegisterMessageHandler(void(UserServerSystem::*method)(network::ConnectionId, const TMessage&)) {
    app_system_.RegisterMessageHandler<TMessage>([this, method](network::ConnectionId id, const TMessage& message) {
      (this->*method)(id, message);
    });
  }

  ApplicationSystem& app_system_;
  UserPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;

  std::unordered_map<network::ConnectionId, api::User> user_sessions_{};
};

}