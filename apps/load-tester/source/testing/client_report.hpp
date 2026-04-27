#pragma once

#include <cstddef>
#include <vector>
#include <chrono>

namespace ebroschin::chatcore::tester {

struct ClientReport {
  std::vector<steady_clock::duration> roundtrip_times;
  std::size_t sent_count;

  [[nodiscard]] std::size_t CompletedCount() const { return roundtrip_times.size(); }
  [[nodiscard]] std::size_t FailedCount() const { return sent_count - CompletedCount(); }
};

}
