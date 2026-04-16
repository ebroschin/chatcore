#include "application_system.h"
#include "ebroschin/core/system_context.h"

namespace ebroschin::chatcore::client {

ApplicationSystem::ApplicationSystem(const core::SystemContext& ctx, core::Application& app) noexcept:
  System(ctx),
  app_(app),
  tcp_system_(ctx.Require<ClientTcpSystem>()),
  message_handler_(tcp_system_.GetMessageProcessor().GetMessageHandler())
{}

void ApplicationSystem::Initialize() {
  application_thread_ = std::jthread{[this](std::stop_token st) {
    auto& processor = tcp_system_.GetMessageProcessor();
    while (!st.stop_requested()) {
      processor.ProcessBlocking();
    }
  }};
}

void ApplicationSystem::Deinitialize() {
  auto& processor = tcp_system_.GetMessageProcessor();
  processor.Stop();

  application_thread_ = {};
}

void ApplicationSystem::Quit() const noexcept {
  app_.Quit();
}

}