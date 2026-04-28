#pragma once

#include <chrono>

namespace ebroschin::chatcore::tester {

struct TrackedMessage {
  std::chrono::steady_clock::time_point time_point;
  std::uint64_t message_id;
};

}