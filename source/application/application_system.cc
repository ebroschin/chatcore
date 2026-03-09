#include "application_system.h"

#include "claw/core/system_context.h"
#include "commons.h"

namespace claw::chat::server {

ApplicationSystem::ApplicationSystem(const core::SystemContext& ctx):
  System(ctx)
{}

void ApplicationSystem::Initialize() {
  auto* tcp_system = ctx_.Get<ChatServerTcpSystem>();
  processor_ = &tcp_system->GetMessageProcessor();
  application_thread_ = std::jthread{[this](std::stop_token st) {
    while (!st.stop_requested()) {
      processor_->ProcessBlocking();
    }
  }};

  tcp_system->Connect({"0.0.0.0", 1338});
}

void ApplicationSystem::Deinitialize() {
  if (!application_thread_.joinable()) return;

  application_thread_.request_stop();
  application_thread_.join();
}

} // namespace claw::chat::server