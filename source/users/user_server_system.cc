#include "../application/chat_persistence_system.h"
#include <claw/network/tcp/tcp_system.h>
#include "adapters/sqlite_user_persistence_adapter.h"
#include "user_server_system.h"

#include <claw/core/system_context.h>

#include <iostream>

namespace claw::chat::server {

UserServerSystem::UserServerSystem(const core::SystemContext& ctx):
  System(ctx),
  adapter_{*ctx.Get<ChatPersistenceSystem>()->Get<UserPersistenceAdapter>()},
  tcp_system_{*ctx.Get<ChatServerTcpSystem>()}
{
  tcp_system_.RegisterMessageHandler<api::CreateUserRequestMessage>([&](network::ConnectionId id, const api::CreateUserRequestMessage& message) {
    const auto user_id = CreateUser(message.name, message.password);
    if (!user_id) {
      auto user = adapter_.GetUser(message.name);
      if (!user) return;

      std::cout << "user already exists: " << user->id << std::endl;
      tcp_system_.Send<api::CreateUserResponseMessage>(id, {user->id, message.name});
      return;
    }

    std::cout << "created user with id: " << *user_id << std::endl;
    tcp_system_.Send<api::CreateUserResponseMessage>(id, {*user_id, message.name});
    tcp_system_.Broadcast<api::PrintMessage>({"A new user has registered: " + message.name + " [" + std::to_string(*user_id) + "]"});
  });

  tcp_system_.RegisterMessageHandler<api::AuthenticateUserRequestMessage>([&](network::ConnectionId id, const api::AuthenticateUserRequestMessage& message) {
    auto result = adapter_.AuthenticateUser(message.name, message.password);
    if (!result.has_value()) {
      tcp_system_.Send<api::PrintMessage>(id, {"Wrong user or password"});
      return;
    }

    api::User& user = result.value();
    user_sessions_.emplace(id, user);
    tcp_system_.Send<api::AuthenticateUserResponseMessage>(id, {user});
    tcp_system_.Broadcast<api::PrintMessage>({user.name + " has logged in"});
  });
}

bool UserServerSystem::ValidateSession(std::uint64_t request_type_id, const network::ConnectionId& id) {
  auto result = user_sessions_.contains(id);
  if (!result) {
    tcp_system_.Send<api::ErrorMessage>(id, {request_type_id, "Not authorized."});
  }

  return result;
}

std::optional<api::PersistenceId> UserServerSystem::CreateUser(const std::string& name, const std::string& password) {
  return adapter_.CreateUser(name, password);
}

std::optional<std::reference_wrapper<const api::User>> UserServerSystem::GetSessionUser(const network::ConnectionId& id) {
  auto it = user_sessions_.find(id);
  if (it == user_sessions_.end()) return std::nullopt;

  return it->second;
}

}

