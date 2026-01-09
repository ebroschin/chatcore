#pragma once

#include "../application/chat_tcp_system.h"
#include "../communication/tcp_system.h"
#include "client_test_system.h"
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
    while (true) {
      if (!running_) continue;

      std::this_thread::sleep_for(std::chrono::seconds(1));
      ctx_.Get<chat::server::ChatServerTcpSystem>()->Broadcast<chat::api::PrintMessage>({"hello from server"});
    }
  }

private:
  std::atomic<bool> running_;
  std::thread worker_;
};

}
