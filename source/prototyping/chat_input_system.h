#pragma once

#include <atomic>
#include <claw/core/system.h>
#include <queue>
#include <thread>

namespace claw::chat::client {

class ChatInputSystem final : public core::System {
public:
  explicit ChatInputSystem(const core::SystemContext& ctx):
    System(ctx),
    running_{false},
    worker_{&ChatInputSystem::UpdateWorker, this}
  {}

  void Initialize() override;
  void Deinitialize() override;

  bool GetLine(std::string& out);

private:
  void UpdateWorker();

  std::atomic<bool> running_;
  std::thread worker_;
  std::queue<std::string> queue_{};
  std::mutex mutex_{};
};

}