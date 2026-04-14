#include "claw/utility/task_thread.h"

namespace claw::utility {

TaskThread::~TaskThread() {
  {
    std::unique_lock lock(mutex_);
    running_ = false;
  }

  if (worker_.joinable()) {
    worker_.join();
  }

  cv_.notify_all();
}

void TaskThread::Post(Task callback) {
  {
    std::unique_lock lock(mutex_);
    queue_.emplace(std::move(callback));
  }
  cv_.notify_one();
}

void TaskThread::ProcessEvents() {
  while (running_) {
    std::unique_lock lock(mutex_);
    cv_.wait(lock, [&]() { return !queue_.empty() || !running_; });

    while (!queue_.empty()) {
      auto callback = std::move(queue_.front());
      queue_.pop();

      lock.unlock();
      callback();
      lock.lock();
    }
  }
}

}