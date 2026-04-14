#pragma once

#include <chrono>

namespace claw::chat::tester {

struct TrackedMessage {
  steady_clock::time_point time_point{};
  std::uint64_t message_id{0};
};

}