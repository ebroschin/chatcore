#pragma once

#include <cstddef>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace ebroschin::chatcore::tester {

using steady_clock = std::chrono::steady_clock;

struct ClientReport {
  std::vector<steady_clock::duration> roundtrip_times;
  std::size_t sent_count;

  [[nodiscard]] std::size_t CompletedCount() const { return roundtrip_times.size(); }
  [[nodiscard]] std::size_t FailedCount() const { return sent_count - CompletedCount(); }

  static steady_clock::duration CalculatePercentile(std::vector<steady_clock::duration> latencies, double normalized_percentage) {
    if (latencies.empty()) return steady_clock::duration::zero();

    const auto fractional_index = std::floor(normalized_percentage * static_cast<double>(latencies.size() - 1));
    const auto index = static_cast<std::size_t>(fractional_index);
    const auto n = latencies.begin() + static_cast<decltype(latencies)::difference_type>(fractional_index);
    std::ranges::nth_element(latencies, n);

    return latencies[index];
  }
};

}
