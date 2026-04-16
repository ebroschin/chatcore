#include <gtest/gtest.h>

#include "../source/application/chat_server_arguments.h"

namespace claw::chat::server::tests {

static ChatServerArguments CreateMockArguments(std::initializer_list<std::string_view> arguments) {
  const std::size_t argc = arguments.size();
  std::vector<char*> argv;
  argv.reserve(argc);

  for (const auto& argument : arguments) {
    argv.emplace_back(const_cast<char*>(argument.begin()));
  }

  return ChatServerArguments(static_cast<int>(argc), argv.data());
}

TEST(ChatServerArgumentsTests, DefaultArguments) {
  const auto args = CreateMockArguments({"chatcore"});
  EXPECT_EQ(args.GetIp(), "0.0.0.0");
  EXPECT_EQ(args.GetPort(), 1338);
  EXPECT_EQ(args.GetSqliteFilename(), "sqlite.db");
  EXPECT_EQ(args.GetLogLevel(), ebroschin::logging::LogLevel::info);
}

TEST(ChatServerArgumentsTests, InvalidPortParse) {
  const auto args = CreateMockArguments({"chatcore", "--port", "abc"});
  EXPECT_EQ(args.GetPort(), 1338);
}

TEST(ChatServerArgumentsTests, InvalidPortOverflow) {
  const auto args = CreateMockArguments({"chatcore", "--port", "999999"});
  EXPECT_EQ(args.GetPort(), 1338);
}

TEST(ChatServerArgumentsTests, ParseLogLevel) {
  const auto verbose = CreateMockArguments({"chatcore", "--log", "verbose"});
  EXPECT_EQ(verbose.GetLogLevel(), ebroschin::logging::LogLevel::verbose);

  const auto debug = CreateMockArguments({"chatcore", "--log", "debug"});
  EXPECT_EQ(debug.GetLogLevel(), ebroschin::logging::LogLevel::debug);

  const auto warning = CreateMockArguments({"chatcore", "--log", "warning"});
  EXPECT_EQ(warning.GetLogLevel(), ebroschin::logging::LogLevel::warning);
}

TEST(ChatServerArgumentsTests, Usecase1) {
  const auto args = CreateMockArguments({"chatcore", "--ip", "0.0.0.0", "--port", "8081", "--log", "critical", "--db", "production.sqlite"});
  EXPECT_EQ(args.GetLogLevel(), ebroschin::logging::LogLevel::critical);
  EXPECT_EQ(args.GetPort(), 8081);
  EXPECT_EQ(args.GetIp(), "0.0.0.0");
  EXPECT_EQ(args.GetSqliteFilename(), "production.sqlite");
}

TEST(ChatServerArgumentsTests, Usecase2) {
  const auto args = CreateMockArguments({"chatcore", "--ip", "0.0.0.0", "--port", "1337", "--log", "verbose", "--db", "local.sqlite"});
  EXPECT_EQ(args.GetLogLevel(), ebroschin::logging::LogLevel::verbose);
  EXPECT_EQ(args.GetPort(), 1337);
  EXPECT_EQ(args.GetIp(), "0.0.0.0");
  EXPECT_EQ(args.GetSqliteFilename(), "local.sqlite");
}

}