#include "chat_server_application.hpp"

#include "../chat/adapters/sqlite_chat_persistence_adapter.hpp"
#include "../chat/chat_server_system.hpp"
#include "../users/adapters/sqlite_user_persistence_adapter.hpp"
#include "../users/user_server_system.hpp"
#include "chat_persistence_system.hpp"
#include "chat_tcp_system.hpp"

#include <ebroschin/logging-modules/spdlog/spdlog-logger.hpp>
#include <ebroschin/logging-modules/stacktrace/boost_stacktrace.hpp>
#include <ebroschin/logging/log.hpp>
#include <ebroschin/persistence/persistence_system.hpp>
#include <ebroschin/scheduling/scheduling_system.hpp>

namespace ebroschin::chatcore::server {

ChatServerApplication::ChatServerApplication(ChatServerArguments arguments):
  arguments_{std::move(arguments)}
{}

void ChatServerApplication::PrepareContext() {
  logging::Log::SetLogger<logging::modules::SpdlogLogger>();
  logging::Log::SetLogLevel(arguments_.GetLogLevel());
  logging::Log::Info("Starting initialization...");

  ctx_.Register<scheduling::SchedulingSystem>();
  ctx_.Register<ChatServerTcpSystem>(executor_);

  auto* persistence_system = ctx_.Register<ChatPersistenceSystem>(arguments_.GetSqliteFilename());
  persistence_system->Register<ChatPersistenceAdapter, SqliteChatPersistenceAdapter>();
  persistence_system->Register<UserPersistenceAdapter, SqliteUserPersistenceAdapter>();

  ctx_.Register<UserServerSystem>(*this);
  ctx_.Register<ChatServerSystem>(*this);
}

void ChatServerApplication::OnContextInitialized() {
  application_thread_ = std::jthread{[this]
  (const std::stop_token& st)
  {
    while (!st.stop_requested()) {
      executor_.ProcessBlocking();
    }
  }};

  RegisterMessageHandler<network::tcp::ConnectionCreated>([](const auto& event) {
    logging::Log::Verbose() << "X Client with id " << *event.connection_id << " has connected";
  });

  RegisterMessageHandler<network::tcp::ConnectionRemoved>([this](const auto& event) {
    auto connection_id = *event.connection_id;
    logging::Log::Verbose() << "X Client with id " << connection_id << " has disconnected";
    ctx_.Require<UserServerSystem>().RemoveSession(connection_id);
  });

  ctx_.Require<ChatServerTcpSystem>().Connect({arguments_.GetIp(), arguments_.GetPort()});
  logging::Log::Info("Accepting clients");
}

void ChatServerApplication::OnContextDeinitialized() {
  executor_.Stop();
  application_thread_ = {};
}

void ChatServerApplication::HandleTerminate() {
  logging::modules::BoostStacktrace::PrintExceptionStacktrace();
  logging::Log::Shutdown();
}

void ChatServerApplication::HandleRpcError(network::ConnectionId connection_id, network::RequestId request_id, const std::string& message) const {
  logging::Log::Debug("RPC Error: " + message);
  ctx_.Require<ChatServerTcpSystem>().Send<api::ErrorResponseMessage>(connection_id, {request_id, message});
}

}