#include "load_tester_application.h"

#include <claw/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.h>
#include <claw/scheduling/scheduling_system.h>

#include <boost/stacktrace.hpp>
#include <iostream>

#include "application_system.h"
#include "client_rpc_system.h"
#include "client_tcp_system.h"

namespace claw::chat::tester {

LoadTesterApplication::LoadTesterApplication(LoadTesterArguments arguments) noexcept:
  arguments_{std::move(arguments)}
{}

void LoadTesterApplication::Initialize() {
  ctx_.Register<ClientTcpSystem>();
  ctx_.Register<scheduling::SchedulingSystem>();

  auto rpc_timeout_handler = network::modules::SchedulerRpcTimeoutHandler{ctx_.Require<scheduling::SchedulingSystem>()};
  ctx_.Register<ClientRpcSystem>(std::move(rpc_timeout_handler));
  ctx_.Register<ApplicationSystem>(*this);
}

void LoadTesterApplication::HandleTerminate() {
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