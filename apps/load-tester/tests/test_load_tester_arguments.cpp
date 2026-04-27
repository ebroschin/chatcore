#include <gtest/gtest.h>

#include "../source/application/load_tester_arguments.hpp"

namespace ebroschin::chatcore::tester::tests {

static LoadTesterArguments CreateMockArguments(std::initializer_list<std::string_view> arguments) {
  const std::size_t argc = arguments.size();
  std::vector<char*> argv;
  argv.reserve(argc);

  for (const auto& argument : arguments) {
    argv.emplace_back(const_cast<char*>(argument.begin()));
  }

  return LoadTesterArguments(static_cast<int>(argc), argv.data());
}

TEST(LoadTesterArgumentsTests, DefaultArguments) {
  const auto args = CreateMockArguments({"chatcore-load-tester"});
  EXPECT_EQ(args.GetIp(), "localhost");
  EXPECT_EQ(args.GetPort(), "1338");
  EXPECT_EQ(args.GetClientCount(), 30);
  EXPECT_EQ(args.GetLogLevel(), ebroschin::logging::LogLevel::info);
}

TEST(LoadTesterArgumentsTests, InvalidClientCountParse) {
  const auto args = CreateMockArguments({"chatcore-load-tester", "--clients", "abc"});
  EXPECT_EQ(args.GetClientCount(), 30);
}

TEST(LoadTesterArgumentsTests, InvalidClientCountOverflow) {
  const auto args = CreateMockArguments({"chatcore-load-tester", "--clients", "99999999999999999999999999"});
  EXPECT_EQ(args.GetClientCount(), 30);
}

TEST(LoadTesterArgumentsTests, InvalidClientCountMissingValue) {
  const auto args = CreateMockArguments({"chatcore-load-tester", "--clients"});
  EXPECT_EQ(args.GetClientCount(), 30);
}

TEST(LoadTesterArgumentsTests, ParseLogLevel) {
  const auto verbose = CreateMockArguments({"chatcore-load-tester", "--log", "verbose"});
  EXPECT_EQ(verbose.GetLogLevel(), ebroschin::logging::LogLevel::verbose);

  const auto debug = CreateMockArguments({"chatcore-load-tester", "--log", "debug"});
  EXPECT_EQ(debug.GetLogLevel(), ebroschin::logging::LogLevel::debug);

  const auto warning = CreateMockArguments({"chatcore-load-tester", "--log", "warning"});
  EXPECT_EQ(warning.GetLogLevel(), ebroschin::logging::LogLevel::warning);

  const auto error = CreateMockArguments({"chatcore-load-tester", "--log", "error"});
  EXPECT_EQ(error.GetLogLevel(), ebroschin::logging::LogLevel::error);

  const auto critical = CreateMockArguments({"chatcore-load-tester", "--log", "critical"});
  EXPECT_EQ(critical.GetLogLevel(), ebroschin::logging::LogLevel::critical);
}

TEST(LoadTesterArgumentsTests, Usecase1) {
  const auto args = CreateMockArguments({"chatcore-load-tester", "--ip", "192.168.178.22", "--port", "8081", "--clients", "100", "--log", "verbose"});
  EXPECT_EQ(args.GetIp(), "192.168.178.22");
  EXPECT_EQ(args.GetPort(), "8081");
  EXPECT_EQ(args.GetClientCount(), 100);
  EXPECT_EQ(args.GetLogLevel(), ebroschin::logging::LogLevel::verbose);
}

TEST(LoadTesterArgumentsTests, Usecase2) {
  const auto args = CreateMockArguments({"chatcore-load-tester", "--ip", "localhost", "--port", "1338", "--clients", "1000", "--log", "critical"});
  EXPECT_EQ(args.GetIp(), "localhost");
  EXPECT_EQ(args.GetPort(), "1338");
  EXPECT_EQ(args.GetClientCount(), 1000);
  EXPECT_EQ(args.GetLogLevel(), ebroschin::logging::LogLevel::critical);
}

}

