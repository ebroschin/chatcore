#pragma once

#include "../communication/tcp_server_system.h"

#include <claw/core/system.h>
#include <claw/core/system_context.h>

namespace claw::prototyping {

class PingServerSystem final : public core::System {
public:
  explicit PingServerSystem(const core::SystemContext& ctx):
    System(ctx), running_{false}, worker_{&PingServerSystem::UpdateWorker, this}
  {}

  void Initialize() override {
    running_ = true;
  }

  void Deinitialize() override {
    running_ = false;
    if (!worker_.joinable()) return;
    worker_.join();
  }

  void UpdateWorker() {
    using clock = std::chrono::steady_clock;
    std::chrono::steady_clock::time_point lastExecution = std::chrono::steady_clock::now();
    
    while (running_) {
      auto now = clock::now();
      if (now - lastExecution >= std::chrono::seconds(1)) {
        lastExecution = now;
        ctx_.Get<communication::TCPServerSystemBase>()->SendMessage("ping");
      }
    }
  }

private:
  std::atomic<bool> running_;
  std::thread worker_;
};

}
