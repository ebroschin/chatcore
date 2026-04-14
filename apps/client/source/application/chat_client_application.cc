#include "chat_client_application.h"

#include <boost/stacktrace.hpp>
#include <iostream>

#include "../commands/client_commands_system.h"
#include "../model/model_system.h"
#include "../session/session_system.h"
#include "../ui/ui_system.h"
#include "application_system.h"
#include "client_rpc_system.h"
#include "client_tcp_system.h"

namespace claw::chat::client {

void ChatClientApplication::Initialize() {
  ctx_.Register<scheduling::SchedulingSystem>();
  ctx_.Register<ClientTcpSystem>();
  ctx_.Register<ApplicationSystem>();

  auto rpc_timeout_handler = network::modules::SchedulerRpcTimeoutHandler{ctx_.Require<scheduling::SchedulingSystem>()};
  ctx_.Register<ClientRpcSystem>(std::move(rpc_timeout_handler));

  ctx_.Register<ModelSystem>();
  ctx_.Register<SessionSystem>();
  ctx_.Register<ClientCommandsSystem>();
  ctx_.Register<UiSystem>(*this);
}

void ChatClientApplication::HandleTerminate() {
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