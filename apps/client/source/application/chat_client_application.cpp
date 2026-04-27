#include "chat_client_application.hpp"

#include "../commands/client_commands_system.hpp"
#include "../model/model_system.hpp"
#include "../session/session_system.hpp"
#include "../ui/ui_system.hpp"
#include "application_system.hpp"
#include "client_rpc_system.hpp"
#include "client_tcp_system.hpp"

#include <ebroschin/logging-modules/stacktrace/boost_stacktrace.hpp>
#include <ebroschin/logging-modules/spdlog/spdlog-logger.hpp>

namespace ebroschin::chatcore::client {

void ChatClientApplication::Initialize() {
  logging::Log::SetLogger<logging::modules::SpdlogLogger>();
  logging::Log::Info("Starting initialization");

  ctx_.Register<scheduling::SchedulingSystem>();
  ctx_.Register<ClientTcpSystem>();
  ctx_.Register<ApplicationSystem>(*this);

  auto rpc_timeout_handler = network::modules::SchedulerRpcTimeoutHandler{ctx_.Require<scheduling::SchedulingSystem>()};
  ctx_.Register<ClientRpcSystem>(std::move(rpc_timeout_handler));

  ctx_.Register<ModelSystem>();
  ctx_.Register<SessionSystem>();
  ctx_.Register<ClientCommandsSystem>();
  ctx_.Register<UiSystem>();
}

void ChatClientApplication::HandleTerminate() {
  logging::Log::Shutdown();

  logging::Log::SetLogger<logging::modules::SpdlogLogger>();
  logging::modules::BoostStacktrace::PrintExceptionStacktrace();
  logging::Log::Shutdown();
}

}