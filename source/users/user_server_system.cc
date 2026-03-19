#include "../application/chat_persistence_system.h"
#include <claw/network/tcp/tcp_system.h>
#include "adapters/sqlite_user_persistence_adapter.h"
#include "user_server_system.h"

#include "../application/application_system.h"
#include "../chat/chat_server_system.h"

#include <claw/core/system_context.h>

namespace claw::chat::server {

UserServerSystem::UserServerSystem(const core::SystemContext& ctx):
  System(ctx),
  app_system_(ctx_.Require<ApplicationSystem>()),
  adapter_(ctx_.Require<ChatPersistenceSystem>().Require<UserPersistenceAdapter>()),
  tcp_system_(ctx_.Require<ChatServerTcpSystem>())
{ }

void UserServerSystem::Initialize() {
  RegisterMessageHandler(&UserServerSystem::HandleCreateUser);
  RegisterMessageHandler(&UserServerSystem::HandleAuthenticateUser);
  RegisterMessageHandler(&UserServerSystem::HandleGetUsers);
  RegisterMessageHandler(&UserServerSystem::HandleLogout);
}

bool UserServerSystem::ValidateSession(network::RequestId request_id, network::ConnectionId id) const {
  const auto result = user_sessions_.contains(id);
  if (!result) {
    tcp_system_.Send(id, api::ErrorResponseMessage{request_id, "Not authorized."});
  }

  return result;
}

bool UserServerSystem::ValidateSession(network::ConnectionId id) const {
  const auto result = user_sessions_.contains(id);
  if (!result) {
    tcp_system_.Send(id, api::ErrorMessage{"Not authorized."});
  }

  return result;
}

void UserServerSystem::RemoveSession(network::ConnectionId connection_id) {
  const auto it = user_sessions_.find(connection_id);
  if (it == user_sessions_.end()) return;

  const auto& user = it->second;
  tcp_system_.Broadcast<api::UserLogoutEventMessage>({user.id});
  user_sessions_.erase(connection_id);
}

std::optional<std::reference_wrapper<const api::User>> UserServerSystem::GetSessionUser(network::ConnectionId connection_id) {
  const auto it = user_sessions_.find(connection_id);
  if (it == user_sessions_.end()) return std::nullopt;

  return it->second;
}

void UserServerSystem::HandleCreateUser(network::ConnectionId connection_id, const api::CreateUserRequestMessage& message) {
  const auto user_id = adapter_.CreateUser(message.name, message.password);
  if (!user_id) {
    const auto user = adapter_.GetUser(message.name); //TODO use cache
    if (!user) {
      tcp_system_.Send<api::ErrorResponseMessage>(connection_id, {message.request_id, "Unexpected error"});
      return;
    }

    tcp_system_.Send(connection_id, api::CreateUserResponseMessage(message.request_id, {user->id, message.name}));
    return;
  }

  tcp_system_.Send(connection_id, api::CreateUserResponseMessage{message.request_id, {*user_id, message.name}});
}

void UserServerSystem::HandleAuthenticateUser(network::ConnectionId connection_id, const api::AuthenticateUserRequestMessage& message) {
  if (user_sessions_.contains(connection_id)) {
    tcp_system_.Send<api::ErrorResponseMessage>(connection_id, {message.request_id, "User already logged in."});
    return;
  }

  const auto result = adapter_.MatchUserCredentials(message.name, message.password);
  if (!result) {
    tcp_system_.Send<api::ErrorResponseMessage>(connection_id, {message.request_id, "Wrong username or password"});
    return;
  }

  const auto& user = *result;
  user_sessions_.emplace(connection_id, user);
  tcp_system_.Send(connection_id, api::AuthenticateUserResponseMessage{message.request_id, user});
  tcp_system_.Broadcast<api::UserLoginEventMessage>({user.id});
}

void UserServerSystem::HandleGetUsers(network::ConnectionId connection_id, const api::GetUsersRequestMessage& message) {
  if (!ValidateSession(message.request_id, connection_id)) return;

  auto result = adapter_.GetUsers(message.user_ids); //TODO use cache
  tcp_system_.Send(connection_id, api::GetUsersResponseMessage{message.request_id, std::move(result)});
}

void UserServerSystem::HandleLogout(network::ConnectionId connection_id, const api::LogoutRequestMessage& message) {
  if (!ValidateSession(message.request_id, connection_id)) return;

  RemoveSession(connection_id);
  tcp_system_.Send(connection_id, api::LogoutResponseMessage{message.request_id});
}

}

