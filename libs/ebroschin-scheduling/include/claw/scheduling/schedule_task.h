#pragma once

#include <chrono>
#include <functional>

#include "commons.h"

using std::chrono::steady_clock;

namespace claw::scheduling {

struct ScheduleTask {
  TaskId id;
  steady_clock::duration interval;
  steady_clock::time_point scheduled_time_point{};
  std::function<void()> callback;
  bool once{false};
};

}