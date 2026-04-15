#include "chat_client_application.h"

#include <ebroschin/logging-modules/stacktrace/boost_stacktrace.hpp>

#include "../commands/client_commands_system.h"
#include "../model/model_system.h"
#include "../session/session_system.h"
#include "../ui/ui_system.h"
#include "application_system.h"
#include "client_rpc_system.h"
#include "client_tcp_system.h"
#include "ebroschin/logging-modules/spdlog/spdlog-logger.hpp"

namespace claw::chat::client {

void ChatClientApplication::Initialize() {
  ebroschin::logging::Log::SetLogger<ebroschin::logging::modules::SpdlogLogger>();
  ebroschin::logging::Log::Info("Starting initialization");

  ctx_.Register<scheduling::SchedulingSystem>();
  ctx_.Register<ClientTcpSystem>();
  ctx_.Register<ApplicationSystem>(*this);

  auto rpc_timeout_handler = network::modules::SchedulerRpcTimeoutHandler{ctx_.Require<scheduling::SchedulingSystem>()};
  ctx_.Register<ClientRpcSystem>(std::move(rpc_timeout_handler));

  ctx_.Register<ModelSystem>();
  ctx_.Register<SessionSystem>();
  ctx_.Register<ClientCommandsSystem>();
  ctx_.Register<UiSystem>(*this);
}

void ChatClientApplication::HandleTerminate() {
  ebroschin::logging::Log::Shutdown();

  ebroschin::logging::Log::SetLogger<ebroschin::logging::modules::SpdlogLogger>();
  ebroschin::logging::modules::BoostStacktrace::PrintExceptionStacktrace();
  ebroschin::logging::Log::Shutdown();
}

}