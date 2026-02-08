#pragma once

#include <claw/core/system.h>
#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <functional>

#include "schedule_task.h"

using std::chrono::steady_clock;

namespace claw::chat::server {

class SchedulingSystem final : public core::System {
public:
  explicit SchedulingSystem(const core::SystemContext& ctx);

  void Initialize() override;
  void Deinitialize() override;
  int AddScheduledTask(steady_clock::duration interval, std::function<void()> task);
  void SetTaskActive(int handle, bool value);

private:
  void ProcessTasks();

  std::atomic<bool> running_{false};
  std::thread scheduler_thread_{};
  std::mutex mutex_{};
  std::condition_variable cv_{};

  std::unordered_map<int, ScheduleTask> tasks_;
};

}