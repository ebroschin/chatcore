#pragma once

#include <condition_variable>
#include <queue>
#include <thread>

namespace claw::communication {

class TcpTaskHandler {
  using Task = std::function<void()>;

public:
  ~TcpTaskHandler() {
    {
      std::unique_lock lock(mutex_);
      running_ = false;
    }

    if (!worker_.joinable()) return;
    worker_.join();

    cv_.notify_all();
  }

  void Post(Task callback) {
    {
      std::unique_lock lock(mutex_);
      queue_.emplace(std::move(callback));
    }
    cv_.notify_one();
  }

private:
  void ProcessEvents() {
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

  bool running_{true};
  std::queue<Task> queue_{};
  std::thread worker_{&TcpTaskHandler::ProcessEvents, this};
  std::mutex mutex_{};
  std::condition_variable cv_{};
};

}