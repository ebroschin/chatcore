#include "chat_server_application.h"

#include "../chat/adapters/sqlite_chat_persistence_adapter.h"
#include "../chat/chat_server_system.h"
#include "../persistence/persistence_system.h"
#include "../prototyping/chat_input_system.h"
#include "../prototyping/client_test_system.h"
#include "../prototyping/ping_server_system.h"
#include "../scheduling/scheduling_system.h"
#include "../sqlite_persistence/sqlite_persistence_store.h"
#include "../users/adapters/sqlite_user_persistence_adapter.h"
#include "../users/user_server_system.h"
#include "chat_persistence_system.h"
#include "chat_tcp_system.h"

#include <boost/stacktrace.hpp>
#include <iostream>

using namespace claw::persistence::sqlite;

namespace claw::chat::server {

void ChatServerApplication::Initialize() {
#ifdef SERVER_SIDE
  auto* persistence_system = ctx_.Register<ChatPersistenceSystem>("sqlite.db3");
  persistence_system->Register<ChatPersistenceAdapter, SqliteChatPersistenceAdapter>();
  persistence_system->Register<UserPersistenceAdapter, SqliteUserPersistenceAdapter>();

  ctx_.Register<ChatServerTcpSystem>();
  ctx_.Register<ChatServerSystem>(*this);
  ctx_.Register<UserServerSystem>();
  ctx_.Register<prototyping::PingServerSystem>();
  ctx_.Register<SchedulingSystem>();
#else
  ctx_.Register<ChatClientTcpSystem>();
  ctx_.Register<client::ChatInputSystem>();
  ctx_.Register<prototyping::ClientTestSystem>();
#endif
}

void ChatServerApplication::HandleTerminate() {
  //TODO COPY PASTE (project visualizer) reuse code from client (move to library)
  try {
    std::rethrow_exception(std::current_exception());
  } catch (const std::exception& e) {
    std::cerr << "Unhandled exception: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unhandled unknown exception" << std::endl;
  }

  std::cerr << "Stacktrace:\n" << boost::stacktrace::stacktrace() << std::endl;
}

}