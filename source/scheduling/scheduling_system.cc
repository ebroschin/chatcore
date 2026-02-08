#include "scheduling_system.h"

#include <ranges>

namespace claw::chat::server {

SchedulingSystem::SchedulingSystem(const core::SystemContext & ctx):
  System(ctx)
{}

void SchedulingSystem::Initialize() {
  running_ = true;
  scheduler_thread_ = std::thread{&SchedulingSystem::ProcessTasks, this};
}

void SchedulingSystem::Deinitialize() {
  running_.store(false, std::memory_order::relaxed);
  cv_.notify_one();

  if (!scheduler_thread_.joinable()) return;
  scheduler_thread_.join();
}

int SchedulingSystem::AddScheduledTask(steady_clock::duration interval, std::function<void()> task) {
  static std::atomic<int> next_id{1};
  auto id = next_id.fetch_add(1, std::memory_order_relaxed);

  {
    std::unique_lock lock(mutex_);
    tasks_.emplace(id, ScheduleTask{false, interval, {}, std::move(task)});
  }
  cv_.notify_one();

  return id;
}

void SchedulingSystem::SetTaskActive(int handle, bool value) {
  {
    std::unique_lock lock(mutex_);
    auto it = tasks_.find(handle);
    if (it == tasks_.end()) return;

    auto& task = it->second;
    bool task_reactivated = !task.active && value;
    task.active = value;
    if (task_reactivated) {
      task.scheduled_time_point = steady_clock::now() + task.interval;
    }
  }
  cv_.notify_one();
}

void SchedulingSystem::ProcessTasks() {
  std::vector<std::function<void()>> callbacks;

  while (running_.load()) {
    std::unique_lock lock(mutex_);

    while (!tasks_.empty()) {
      callbacks.clear();

      auto current_time_point = steady_clock::now();
      auto next_wake_time = steady_clock::time_point::max();
      for (auto& task : tasks_ | std::views::values) {
        if (!task.active) continue;

        if (current_time_point >= task.scheduled_time_point) {
          callbacks.push_back(task.callback);
          task.scheduled_time_point = current_time_point + task.interval;
        }

        next_wake_time = std::min(next_wake_time, task.scheduled_time_point);
      }

      lock.unlock();
      for (const auto& callback : callbacks) {
        callback();
      }
      lock.lock();

      cv_.wait_until(lock, next_wake_time);
      if (!running_.load()) return;
    }

    cv_.wait(lock, [&] { return !tasks_.empty() || !running_; });
    if (!running_.load()) return;
  }
}


}