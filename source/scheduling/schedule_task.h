#pragma once

#include <chrono>
#include <functional>

using std::chrono::steady_clock;

namespace claw::chat::server {

struct ScheduleTask {
  bool active{false};
  steady_clock::duration interval;
  steady_clock::time_point scheduled_time_point{};
  std::function<void()> callback;
};

}