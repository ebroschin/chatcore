#include "user_server_system.hpp"
#include "../application/chat_persistence_system.hpp"
#include "adapters/sqlite_user_persistence_adapter.hpp"
#include <ebroschin/network/tcp/tcp_system.hpp>

#include "../application/application_system.hpp"
#include <ebroschin/core/system_context.hpp>

namespace ebroschin::chatcore::server {

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
  app_system_.RegisterMessageHandler(this, &UserServerSystem::HandleGetUser);

  user_store_.Prewarm();
}

bool UserServerSystem::ValidateSession(network::RequestId request_id, network::ConnectionId connection_id) const {
  const auto result = user_store_.HasSession(connection_id);
  if (!result) {
    app_system_.HandleRpcError(connection_id, request_id, "Not authorized.");
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
    app_system_.HandleRpcError(connection_id, message.request_id, "User already exists.");
    return;
  }

  tcp_system_.Send<api::CreateUserResponseMessage>(connection_id, {message.request_id, {*result}});
}

void UserServerSystem::HandleAuthenticateUser(network::ConnectionId connection_id, const api::AuthenticateUserRequestMessage& message) {
  if (user_store_.HasSession(connection_id)) {
    app_system_.HandleRpcError(connection_id, message.request_id, "Already logged in.");
    return;
  }

  const auto result = adapter_.MatchUserCredentials(message.name, message.password);
  if (!result) {
    app_system_.HandleRpcError(connection_id, message.request_id, "Wrong username or password");
    return;
  }

  const auto& user = *result;
  if (user_store_.HasSession(user.id)) {
    app_system_.HandleRpcError(connection_id, message.request_id, message.name + " is already logged in.");
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

void UserServerSystem::HandleGetUser(network::ConnectionId connection_id, const api::GetUserRequestMessage& message) {
  if (!ValidateSession(message.request_id, connection_id)) return;

  const auto result = user_store_.GetUser(message.name);
  if (!result) {
    app_system_.HandleRpcError(connection_id, message.request_id, "User with name " + message.name + " not found");
    return;
  }
  tcp_system_.Send<api::GetUserResponseMessage>(connection_id, {message.request_id, result->get()});
}

}

