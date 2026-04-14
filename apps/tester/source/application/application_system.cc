#include "application_system.h"

#include <claw/core/system_context.h>

#include <iostream>

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
  application_thread_ = {};
}

}