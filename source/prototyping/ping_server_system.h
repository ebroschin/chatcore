#pragma once

#include "../communication/tcp_system.h"

#include <claw/core/system.h>
#include <claw/core/system_context.h>
#include "../application/chat_tcp_system.h"

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
    
    while (true) {
      if (!running_) continue;

      auto now = clock::now();
      if (now - lastExecution >= std::chrono::seconds(1)) {
        lastExecution = now;
        ctx_.Get<chat::server::ChatServerTcpSystem>()->SendMessage("ping:hello from server");
      }
    }
  }

private:
  std::atomic<bool> running_;
  std::thread worker_;
};

}
