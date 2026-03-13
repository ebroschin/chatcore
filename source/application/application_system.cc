#include "application_system.h"

#include "chat_server_application.h"
#include "claw/core/system_context.h"
#include "commons.h"

namespace claw::chat::server {

ApplicationSystem::ApplicationSystem(const core::SystemContext& ctx, ChatServerApplication& app):
  System(ctx),
  app_(app),
  tcp_system_(ctx.Require<ChatServerTcpSystem>()),
  message_handler_(tcp_system_.GetMessageProcessor().GetMessageHandler())
{}

void ApplicationSystem::Initialize() {
  auto* tcp_system = ctx_.Get<ChatServerTcpSystem>();
  application_thread_ = std::jthread{[this](std::stop_token st) {
    auto& processor = tcp_system_.GetMessageProcessor();
    while (!st.stop_requested()) {
      processor.ProcessBlocking();
    }
  }};

  tcp_system->Connect({"0.0.0.0", 1338});
}

void ApplicationSystem::Deinitialize() {
  application_thread_ = {};
}

void ApplicationSystem::Shutdown() const noexcept {
  app_.Quit();
}

}