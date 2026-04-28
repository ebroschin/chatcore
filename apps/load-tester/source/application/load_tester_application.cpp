#include "load_tester_application.hpp"

#include "application_system.hpp"
#include "client_rpc_system.hpp"
#include "client_tcp_system.hpp"

#include <ebroschin/scheduling/scheduling_system.hpp>
#include <ebroschin/logging-modules/spdlog/spdlog-logger.hpp>
#include <ebroschin/logging-modules/stacktrace/boost_stacktrace.hpp>
#include <ebroschin/logging/log.hpp>

namespace ebroschin::chatcore::tester {

LoadTesterApplication::LoadTesterApplication(LoadTesterArguments arguments) noexcept:
  arguments_{std::move(arguments)}
{}

void LoadTesterApplication::Initialize() {
  logging::Log::SetLogger<logging::modules::SpdlogLogger>();
  logging::Log::SetLogLevel(arguments_.GetLogLevel());
  logging::Log::Info("Starting initialization");

  ctx_.Register<ClientTcpSystem>();

  auto* scheduling_system = ctx_.Register<scheduling::SchedulingSystem>();
  ctx_.Register<ClientRpcSystem>(*scheduling_system);

  ctx_.Register<ApplicationSystem>(*this);
}

void LoadTesterApplication::HandleTerminate() {
  logging::modules::BoostStacktrace::PrintExceptionStacktrace();
  logging::Log::Shutdown();
}

}