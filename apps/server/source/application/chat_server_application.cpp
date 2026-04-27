#include "chat_server_application.hpp"

#include <ebroschin/logging-modules/spdlog/spdlog-logger.hpp>
#include <ebroschin/logging-modules/stacktrace/boost_stacktrace.hpp>
#include <ebroschin/logging/log.hpp>
#include <ebroschin/persistence-modules/sqlite/sqlite_persistence_store.hpp>
#include <ebroschin/persistence/persistence_system.hpp>
#include <ebroschin/scheduling/scheduling_system.hpp>

#include "../chat/adapters/sqlite_chat_persistence_adapter.hpp"
#include "../chat/chat_server_system.hpp"
#include "../users/adapters/sqlite_user_persistence_adapter.hpp"
#include "../users/user_server_system.hpp"
#include "application_system.hpp"
#include "chat_persistence_system.hpp"
#include "chat_tcp_system.hpp"

using namespace ebroschin::persistence::modules::sqlite;

namespace ebroschin::chatcore::server {

ChatServerApplication::ChatServerApplication(ChatServerArguments arguments):
  arguments_(std::move(arguments))
{ }

void ChatServerApplication::Initialize() {
  logging::Log::SetLogger<logging::modules::SpdlogLogger>();
  logging::Log::SetLogLevel(arguments_.GetLogLevel());
  logging::Log::Info("Starting initialization");

  ctx_.Register<scheduling::SchedulingSystem>();
  ctx_.Register<ChatServerTcpSystem>();

  auto* persistence_system = ctx_.Register<ChatPersistenceSystem>(arguments_.GetSqliteFilename());
  persistence_system->Register<ChatPersistenceAdapter, SqliteChatPersistenceAdapter>();
  persistence_system->Register<UserPersistenceAdapter, SqliteUserPersistenceAdapter>();

  ctx_.Register<ApplicationSystem>(*this);
  ctx_.Register<UserServerSystem>();
  ctx_.Register<ChatServerSystem>();
}

void ChatServerApplication::HandleTerminate() {
  logging::modules::BoostStacktrace::PrintExceptionStacktrace();
  logging::Log::Shutdown();
}

}