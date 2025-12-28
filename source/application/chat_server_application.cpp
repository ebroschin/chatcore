#include "chat_server_application.h"

#include "../boost_communication/boost_tcp_server.h"
#include "../chat/adapters/sqlite_chat_persistence_adapter.h"
#include "../chat/chat_server_system.h"
#include "../communication/tcp_server_system.h"
#include "../persistence/persistence_system.h"
#include "../prototyping/client_communication_system.h"
#include "../prototyping/prototyping_system.h"
#include "../sqlite_persistence/sqlite_persistence_store.h"

#include <boost/stacktrace.hpp>
#include <iostream>

using namespace claw::persistence::sqlite;

#define SERVER_SIDE

namespace claw::chat::server {

void ChatServerApplication::Initialize() {
  ctx_->Register<prototyping::PrototypingSystem>(argument_);

#ifdef SERVER_SIDE
  auto* persistence_system = ctx_->Register<persistence::PersistenceSystemBase, persistence::PersistenceSystem<SqlitePersistenceStore>>("sqlite.db3");
  static_cast<persistence::PersistenceSystem<SqlitePersistenceStore>*>(persistence_system)->Register<ChatPersistenceAdapter, SqliteChatPersistenceAdapter>();

  ctx_->Register<communication::TCPServerSystemBase, communication::TCPServerSystem<communication::BoostTCPServer>>(*this, "0.0.0.0", 1338);
  ctx_->Register<ChatServerSystem>();
#else
  ctx_->Register<communication::ClientCommunicationSystem>(*this);
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