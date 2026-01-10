#include "../application/chat_persistence_system.h"
#include "../communication/tcp_system.h"
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
  tcp_system_.RegisterMessageHandler<api::CreateUserMessage>([&](communication::ConnectionID, const api::CreateUserMessage& message) {
    const auto user_id = CreateUser(message.name, message.password);
    std::cout << "created user with id: " << user_id << std::endl;

    tcp_system_.Broadcast<api::PrintMessage>({"A new user has registered: " + message.name + " [" + std::to_string(user_id) + "]"});
  });

  tcp_system_.RegisterMessageHandler<api::AuthenticateUserRequestMessage>([&](communication::ConnectionID id, const api::AuthenticateUserRequestMessage& message) {
    auto result = adapter_.AuthenticateUser(message.name, message.password);
    if (!result.has_value()) {
      tcp_system_.Send<api::PrintMessage>(id, {"Wrong user or password"});
      return;
    }

    api::User& user = result.value();
    user_sessions_.emplace(id, user);
    tcp_system_.Broadcast<api::PrintMessage>({"User has logged in: " + user.name + " [" + std::to_string(user.id) + "]"});
  });
}

bool UserServerSystem::ValidateSession(const communication::ConnectionID& id) {
  auto result = user_sessions_.contains(id);
  if (!result) {
    tcp_system_.Send<api::PrintMessage>(id, {"Not authorized."});
  }
  return result;
}

api::PersistenceID UserServerSystem::CreateUser(const std::string& name, const std::string& password) {
  return adapter_.CreateUser(name, password);
}

std::optional<std::reference_wrapper<const api::User>> UserServerSystem::GetSessionUser(const communication::ConnectionID& id) {
  auto it = user_sessions_.find(id);
  if (it == user_sessions_.end()) return std::nullopt;

  return it->second;
}

}

