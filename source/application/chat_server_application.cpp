#include "chat_server_application.h"

#include "../boost_communication/boost_tcp_server.h"
#include "../boost_communication/boost_tcp_client.h"
#include "../chat/adapters/sqlite_chat_persistence_adapter.h"
#include "../chat/chat_server_system.h"
#include "../communication/tcp_system.h"
#include "../persistence/persistence_system.h"
#include "../prototyping/chat_input_system.h"
#include "../prototyping/ping_server_system.h"
#include "../prototyping/prototyping_system.h"
#include "../sqlite_persistence/sqlite_persistence_store.h"
#include "../prototyping/client_test_system.h"

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

  ctx_->Register<communication::TcpSystem, communication::TcpServerSystem<communication::BoostTcpServer, communication::BoostTcpServer::ConnectionType>>("0.0.0.0", 1338);
  ctx_->Register<ChatServerSystem>();
  ctx_->Register<prototyping::PingServerSystem>();
#else
  ctx_->Register<communication::TcpSystem, communication::TcpClientSystem<communication::BoostTcpClient, communication::BoostTcpClient::ConnectionType>>(*this, "localhost", 1338);
  ctx_->Register<prototyping::ClientTestSystem>();
  ctx_->Register<client::ChatInputSystem>();
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