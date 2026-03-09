#include "../application/chat_persistence_system.h"
#include <claw/network/tcp/tcp_system.h>
#include "adapters/sqlite_user_persistence_adapter.h"
#include "user_server_system.h"

#include "../application/application_system.h"

#include <claw/core/system_context.h>

namespace claw::chat::server {

UserServerSystem::UserServerSystem(const core::SystemContext& ctx):
  System(ctx),
  adapter_(ctx_.Require<ChatPersistenceSystem>().Require<UserPersistenceAdapter>()),
  tcp_system_(ctx_.Require<ChatServerTcpSystem>())
{ }

void UserServerSystem::Initialize() {
  auto& application_system = ctx_.Require<ApplicationSystem>();
  application_system.RegisterMessageHandler<api::CreateUserRequestMessage>([&](network::ConnectionId id, const api::CreateUserRequestMessage& message) {
    const auto user_id = adapter_.CreateUser(message.name, message.password);
    if (!user_id) {
      const auto user = adapter_.GetUser(message.name);
      if (!user) return;

      tcp_system_.Send(id, api::CreateUserResponseMessage(message.request_id, {user->id, message.name}));
      return;
    }

    tcp_system_.Send(id, api::CreateUserResponseMessage{message.request_id, {*user_id, message.name}});
    tcp_system_.Broadcast<api::PrintMessage>({"A new user has registered: " + message.name + " [" + std::to_string(*user_id) + "]"});
  });

  application_system.RegisterMessageHandler<api::AuthenticateUserRequestMessage>([&](network::ConnectionId id, const api::AuthenticateUserRequestMessage& message) {
    const auto result = adapter_.AuthenticateUser(message.name, message.password);
    if (!result.has_value()) {
      tcp_system_.Send<api::PrintMessage>(id, {"Wrong user or password"});
      return;
    }

    const api::User& user = result.value();
    user_sessions_.emplace(id, user);
    tcp_system_.Send(id, api::AuthenticateUserResponseMessage{message.request_id, user});
    tcp_system_.Broadcast<api::PrintMessage>({user.name + " has logged in"});
  });
}

bool UserServerSystem::ValidateSession(network::RequestId request_id, const network::ConnectionId& id) const {
  const auto result = user_sessions_.contains(id);
  if (!result) {
    tcp_system_.Send(id, api::ErrorResponseMessage{request_id, "Not authorized."});
  }

  return result;
}

bool UserServerSystem::ValidateSession(const network::ConnectionId& id) const {
  const auto result = user_sessions_.contains(id);
  if (!result) {
    tcp_system_.Send(id, api::ErrorMessage{"Not authorized."});
  }

  return result;
}

std::optional<std::reference_wrapper<const api::User>> UserServerSystem::GetSessionUser(const network::ConnectionId& id) {
  const auto it = user_sessions_.find(id);
  if (it == user_sessions_.end()) return std::nullopt;

  return it->second;
}

}

