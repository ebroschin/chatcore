#include "../application/chat_persistence_system.h"
#include <claw/network/tcp/tcp_system.h>
#include "adapters/sqlite_user_persistence_adapter.h"
#include "user_server_system.h"

#include "../application/application_system.h"
#include <claw/core/system_context.h>

namespace claw::chat::server {

UserServerSystem::UserServerSystem(const core::SystemContext& ctx):
  System(ctx),
  app_system_(ctx_.Require<ApplicationSystem>()),
  adapter_(ctx_.Require<ChatPersistenceSystem>().Require<UserPersistenceAdapter>()),
  tcp_system_(ctx_.Require<ChatServerTcpSystem>())
{ }

void UserServerSystem::Initialize() {
  app_system_.RegisterMessageHandler(this, &UserServerSystem::HandleCreateUser);
  app_system_.RegisterMessageHandler(this, &UserServerSystem::HandleAuthenticateUser);
  app_system_.RegisterMessageHandler(this, &UserServerSystem::HandleGetUsers);

  user_store_.Prewarm();
}

bool UserServerSystem::ValidateSession(network::RequestId request_id, network::ConnectionId id) const {
  const auto result = user_store_.HasSession(id);
  if (!result) {
    tcp_system_.Send<api::ErrorResponseMessage>(id, {request_id, "Not authorized."});
  }

  return result;
}

bool UserServerSystem::ValidateSession(network::ConnectionId id) const {
  const auto result = user_store_.HasSession(id);
  if (!result) {
    tcp_system_.Send<api::ErrorMessage>(id, {"Not authorized."});
  }

  return result;
}

void UserServerSystem::RemoveSession(network::ConnectionId connection_id) {
  const auto potential_user = user_store_.GetSessionUser(connection_id);
  if (!potential_user) return;

  const auto& user = potential_user->get();
  tcp_system_.Broadcast<api::UserLogoutEventMessage>({user.id});
  user_store_.RemoveSession(connection_id);
}

std::optional<std::reference_wrapper<const api::User>> UserServerSystem::GetSessionUser(network::ConnectionId connection_id) {
  return user_store_.GetSessionUser(connection_id);
}

void UserServerSystem::HandleCreateUser(network::ConnectionId connection_id, const api::CreateUserRequestMessage& message) {
  const auto result = adapter_.CreateUser(message.name, message.password);
  if (!result) {
    const auto user = user_store_.GetUser(message.name);
    if (!user) {
      tcp_system_.Send<api::ErrorResponseMessage>(connection_id, {message.request_id, "Unexpected error"});
      return;
    }

    tcp_system_.Send<api::CreateUserResponseMessage>(connection_id, {message.request_id, user->get()});
    return;
  }

  tcp_system_.Send<api::CreateUserResponseMessage>(connection_id, {message.request_id, {*result}});
}

void UserServerSystem::HandleAuthenticateUser(network::ConnectionId connection_id, const api::AuthenticateUserRequestMessage& message) {
  if (user_store_.HasSession(connection_id)) {
    tcp_system_.Send<api::ErrorResponseMessage>(connection_id, {message.request_id, "Already logged in."});
    return;
  }

  const auto result = adapter_.MatchUserCredentials(message.name, message.password);
  if (!result) {
    tcp_system_.Send<api::ErrorResponseMessage>(connection_id, {message.request_id, "Wrong username or password"});
    return;
  }

  const auto& user = *result;
  if (user_store_.HasSession(user.id)) {
    tcp_system_.Send<api::ErrorResponseMessage>(connection_id, {message.request_id, message.name + " is already logged in."});
    return;
  }

  user_store_.AssignSession(connection_id, user.id);
  tcp_system_.Send<api::AuthenticateUserResponseMessage>(connection_id, {message.request_id, user});
  tcp_system_.Broadcast<api::UserLoginEventMessage>({user.id});
}

void UserServerSystem::HandleGetUsers(network::ConnectionId connection_id, const api::GetUsersRequestMessage& message) {
  if (!ValidateSession(message.request_id, connection_id)) return;

  auto result = user_store_.GetUsers(message.user_ids);
  tcp_system_.Send<api::GetUsersResponseMessage>(connection_id, {message.request_id, std::move(result)});
}

}

