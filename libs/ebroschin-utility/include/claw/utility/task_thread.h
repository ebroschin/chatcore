#pragma once

#include <functional>
#include <condition_variable>
#include <queue>
#include <thread>

namespace claw::utility {

class TaskThread {
  using Task = std::function<void()>;

public:
  ~TaskThread();
  void Post(Task callback);

private:
  void ProcessEvents();

  bool running_{true};
  std::queue<Task> queue_{};
  std::thread worker_{&TaskThread::ProcessEvents, this};
  std::mutex mutex_{};
  std::condition_variable cv_{};
};

}