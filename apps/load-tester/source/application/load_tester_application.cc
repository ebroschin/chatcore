#include "load_tester_application.h"

#include <ebroschin/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.h>
#include <ebroschin/scheduling/scheduling_system.h>

#include <ebroschin/logging-modules/spdlog/spdlog-logger.hpp>
#include <ebroschin/logging-modules/stacktrace/boost_stacktrace.hpp>
#include <ebroschin/logging/log.hpp>

#include "application_system.h"
#include "client_rpc_system.h"
#include "client_tcp_system.h"

namespace ebroschin::chatcore::tester {

LoadTesterApplication::LoadTesterApplication(LoadTesterArguments arguments) noexcept:
  arguments_{std::move(arguments)}
{}

void LoadTesterApplication::Initialize() {
  logging::Log::SetLogger<ebroschin::logging::modules::SpdlogLogger>();
  logging::Log::SetLogLevel(arguments_.GetLogLevel());
  logging::Log::Info("Starting initialization");

  ctx_.Register<ClientTcpSystem>();
  auto* scheduling_system = ctx_.Register<scheduling::SchedulingSystem>();

  auto rpc_timeout_handler = network::modules::SchedulerRpcTimeoutHandler{*scheduling_system};
  ctx_.Register<ClientRpcSystem>(std::move(rpc_timeout_handler));
  ctx_.Register<ApplicationSystem>(*this);
}

void LoadTesterApplication::HandleTerminate() {
  ebroschin::logging::modules::BoostStacktrace::PrintExceptionStacktrace();
  logging::Log::Shutdown();
}

}