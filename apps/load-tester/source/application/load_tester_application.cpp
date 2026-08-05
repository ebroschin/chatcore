#include "load_tester_application.hpp"

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

void LoadTesterApplication::PrepareContext() {
  logging::Log::SetLogger<logging::modules::SpdlogLogger>();
  logging::Log::SetLogLevel(arguments_.GetLogLevel());
  logging::Log::Info("Starting initialization");

  ctx_.Register<ClientTcpSystem>(executor_);

  auto* scheduling_system = ctx_.Register<scheduling::SchedulingSystem>();
  ctx_.Register<ClientRpcSystem>(*scheduling_system);
}

void LoadTesterApplication::OnContextInitialized() {
  application_thread_ = std::jthread{[this]
  (const std::stop_token& st)
  {
    while (!st.stop_requested()) {
      executor_.ProcessBlocking();
    }
  }};

  root_client_ = std::make_unique<RootClient>(*this, ctx_, "root-tester");
  root_client_->Prepare();
}

void LoadTesterApplication::OnContextDeinitialized() {
  executor_.Stop();
  application_thread_ = {};
}

void LoadTesterApplication::HandleTerminate() {
  logging::modules::BoostStacktrace::PrintExceptionStacktrace();
  logging::Log::Shutdown();
}

}