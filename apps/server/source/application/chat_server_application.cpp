#include "chat_server_application.h"

#include "../chat/adapters/sqlite_chat_persistence_adapter.h"
#include "../chat/chat_server_system.h"
#include "../persistence/persistence_system.h"
#include "../sqlite_persistence/sqlite_persistence_store.h"
#include "../users/adapters/sqlite_user_persistence_adapter.h"
#include "../users/user_server_system.h"
#include "application_system.h"
#include "chat_persistence_system.h"
#include "chat_tcp_system.h"
#include <claw/scheduling/scheduling_system.h>

#include <boost/stacktrace.hpp>
#include <iostream>
#include <ebroschin/logging-modules/spdlog/spdlog-logger.hpp>
#include <ebroschin/logging/log.hpp>

using namespace claw::persistence::sqlite;

namespace claw::chat::server {

ChatServerApplication::ChatServerApplication(ChatServerArguments arguments):
  arguments_(std::move(arguments))
{ }

void ChatServerApplication::Initialize() {
  ebroschin::logging::Log::SetLogger<ebroschin::logging::modules::SpdlogLogger>();
  ebroschin::logging::Log::SetLogLevel(arguments_.GetLogLevel());
  ebroschin::logging::Log::Info("Starting initialization");

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
  try {
    std::rethrow_exception(std::current_exception());
  } catch (const std::exception& e) {
    std::cerr << "Unhandled exception" << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unhandled unknown exception" << std::endl;
  }

  std::cerr << "Stacktrace:\n" << stacktrace::stacktrace() << std::endl;
}

}