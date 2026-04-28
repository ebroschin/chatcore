#include <gtest/gtest.h>

#include "../source/testing/client_report.hpp"

#include <chrono>
#include <vector>

namespace ebroschin::chatcore::tester::tests {

TEST(RootClientReportingTests, CalculatePercentile) {
  using namespace std::chrono_literals;
  using Duration = std::chrono::steady_clock::duration;

  const auto us = [](int value) {
    return std::chrono::duration_cast<Duration>(std::chrono::microseconds{value});
  };

  std::vector<Duration> latencies{};
  latencies.reserve(1000);

  for (int i = 1; i <= 1000; i++) {
    latencies.emplace_back(us(i));
  }

  EXPECT_EQ(ClientReport::CalculatePercentile({}, 0.5), Duration::zero());
  EXPECT_EQ(ClientReport::CalculatePercentile(latencies, 0.5), us(500));

  for (int i = 1; i <= 1000; i++) {
    EXPECT_EQ(ClientReport::CalculatePercentile(latencies, i / 1000.0), us(i));
  }
}

}