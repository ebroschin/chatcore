#include "application_system.h"
#include "claw/core/system_context.h"

namespace claw::chat::client {

ApplicationSystem::ApplicationSystem(const core::SystemContext& ctx) noexcept:
  System(ctx),
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
  application_thread_ = {};
}

}