#include "application_system.h"

#include <claw/core/system_context.h>

#include "chat_server_application.h"
#include "chat_tcp_system.h"

namespace claw::chat::server {

ApplicationSystem::ApplicationSystem(const core::SystemContext& ctx, ChatServerApplication& app):
  System{ctx},
  app_{app},
  tcp_system_{ctx.Require<ChatServerTcpSystem>()},
  message_handler_{tcp_system_.GetMessageProcessor().GetMessageHandler()}
{}

void ApplicationSystem::Initialize() {
  application_thread_ = std::jthread{[this](std::stop_token st) {
    auto& processor = tcp_system_.GetMessageProcessor();
    while (!st.stop_requested()) {
      processor.ProcessBlocking();
    }
  }};

  const auto& arguments = app_.GetArguments();
  tcp_system_.Connect({arguments.GetIp(), arguments.GetPort()});
}

void ApplicationSystem::Deinitialize() {
  application_thread_ = {};
}

void ApplicationSystem::Shutdown() const noexcept {
  app_.Quit();
}

}