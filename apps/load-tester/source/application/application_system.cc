#include "application_system.h"

#include <claw/core/system_context.h>

namespace claw::chat::tester {
ApplicationSystem::ApplicationSystem(const core::SystemContext& ctx, LoadTesterApplication& app):
  System(ctx),
  app_{app},
  tcp_system_{ctx.Require<ClientTcpSystem>()},
  rpc_system_{ctx.Require<ClientRpcSystem>()},
  message_handler_{tcp_system_.GetMessageProcessor().GetMessageHandler()}
{}

void ApplicationSystem::Initialize() {
  application_thread_ = std::jthread{[this](std::stop_token st) {
    auto& processor = tcp_system_.GetMessageProcessor();
    while (!st.stop_requested()) {
      processor.ProcessBlocking();
    }
  }};

  auto& scheduling_system = ctx_.Require<scheduling::SchedulingSystem>();
  root_client_ = std::make_unique<RootClient>(*this, tcp_system_, rpc_system_, scheduling_system, "root-tester");
  root_client_->Prepare();
}

void ApplicationSystem::Deinitialize() {
  auto& processor = tcp_system_.GetMessageProcessor();
  processor.Stop();

  application_thread_ = {};
}

void ApplicationSystem::Quit() const {
  if (!app_.IsRunning()) return;
  ebroschin::logging::Log::Info() << "Shutting down";
  ebroschin::logging::Log::Shutdown();
  app_.Quit();
}

}