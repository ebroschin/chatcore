#pragma once

#include <claw/network/tcp/tcp_system.h>
#include "client_test_system.h"
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

    auto* channel = ctx_.Get<chat::server::ChatServerTcpSystem>()->CreateMessageChannel();
    worker_ = std::thread{[this, channel]() {
      while (running_) {
        channel->ProcessBlocking();
      }
    }};
  }

  void Deinitialize() override {
    running_ = false;
    if (!worker_.joinable()) return;
    worker_.join();
  }

  void UpdateWorker() {
    // while (true) {
    //   if (!running_) continue;
    //
    //   std::this_thread::sleep_for(std::chrono::seconds(1));
    //   ctx_.Get<chat::server::ChatServerTcpSystem>()->Broadcast<chat::api::PrintMessage>({"hello from server"});
    // }
  }

private:
  std::atomic<bool> running_;
  std::thread worker_{};
};

}
