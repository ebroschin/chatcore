#pragma once

#include <claw/network/tcp/tcp_system.h>
#include <claw/core/system.h>
#include <claw/core/system_context.h>

namespace claw::prototyping {

class PingServerSystem final : public core::System {
public:
  explicit PingServerSystem(const core::SystemContext& ctx):
    System(ctx), running_{false}
  {}

  void Initialize() override {
    running_ = true;

    auto* processor = ctx_.Get<chat::server::ChatServerTcpSystem>()->CreateMessageProcessor();
    worker_ = std::thread{[this, processor]() {
      while (running_) {
        processor->ProcessBlocking();
      }
    }};
  }

  void Deinitialize() override {
    running_ = false;
    if (!worker_.joinable()) return;
    worker_.join();
  }

private:
  std::atomic<bool> running_;
  std::thread worker_{};
};

}
