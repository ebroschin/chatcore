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

#include "../prototyping/fallback_message_handler.h"
#include "chat_persistence_system.h"
#include "chat_tcp_system.h"
#include <boost/stacktrace.hpp>
#include <iostream>

using namespace claw::persistence::sqlite;

#define SERVER_SIDE

namespace claw::chat::server {

void ChatServerApplication::Initialize() {
  ctx_->Register<prototyping::PrototypingSystem>(argument_);

#ifdef SERVER_SIDE
  auto* persistence_system = ctx_->Register<ChatPersistenceSystem>("sqlite.db3");
  persistence_system->Register<ChatPersistenceAdapter, SqliteChatPersistenceAdapter>();

  auto* tcp_system = ctx_->Register<ChatServerTcpSystem>();
  ctx_->Register<ChatServerSystem>();
  ctx_->Register<prototyping::PingServerSystem>();

  // tcp_system->RegisterFallbackMessageHandler<prototyping::FallbackMessageHandler>();
  tcp_system->Connect({"0.0.0.0", 1338});
#else
  auto* tcp_system = ctx_->Register<ChatClientTcpSystem>();
  ctx_->Register<client::ChatInputSystem>();
  ctx_->Register<prototyping::ClientTestSystem>();

  //tcp_system->RegisterFallbackMessageHandler<prototyping::FallbackMessageHandler>();
  tcp_system->Connect({"localhost", "1338"});
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