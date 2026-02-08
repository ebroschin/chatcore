#pragma once

#include "../scheduling/scheduling_system.h"

#include <claw/core/system.h>
#include <claw/core/system_context.h>
#include <claw/network/tcp/tcp_system.h>
#include <iostream>

using namespace std::chrono_literals;

namespace claw::prototyping {

class PingServerSystem final : public core::System {
public:
  explicit PingServerSystem(const core::SystemContext& ctx):
    System(ctx)
  {}

  void Initialize() override {
    running_ = true;

    auto* tcp_system = ctx_.Get<chat::server::ChatServerTcpSystem>();
    auto* processor = tcp_system->CreateMessageProcessor();
    worker_ = std::thread{[this, processor] {
      while (running_) {
        processor->ProcessBlocking();
      }
    }};

    tcp_system->Connect({"0.0.0.0", 1338});
  }

  void Deinitialize() override {
    running_ = false;
    if (!worker_.joinable()) return;
    worker_.join();
  }

private:
  std::atomic<bool> running_{false};
  std::thread worker_{};
};

}
