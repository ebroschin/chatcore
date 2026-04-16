#include "load_tester_arguments.h"

#include <limits>

namespace ebroschin::chatcore::tester {

LoadTesterArguments::LoadTesterArguments(int argc, char** argv) {
  const utility::Arguments arguments{argc, argv};

  ip_ = ParseIp(arguments).value_or("localhost");
  port_ = ParsePort(arguments).value_or("1338");
  client_count_ = ParseClientCount(arguments).value_or(30);
  log_level_ = ParseLogLevel(arguments).value_or(logging::LogLevel::info);
}

std::optional<std::string> LoadTesterArguments::ParseIp(const utility::Arguments& arguments) {
  const auto values = arguments.GetValues("ip");
  if (!values || values->empty()) return std::nullopt;

  return values->front();
}

std::optional<std::string> LoadTesterArguments::ParsePort(const utility::Arguments& arguments) {
  const auto values = arguments.GetValues("port");
  if (!values || values->empty()) return std::nullopt;

  return values->front();
}

std::optional<unsigned int> LoadTesterArguments::ParseClientCount(const utility::Arguments& arguments) {
  const auto values = arguments.GetValues("clients");
  if (!values || values->empty()) return std::nullopt;

  try {
    const auto result = std::stoul(values->front());
    if (result > std::numeric_limits<unsigned int>::max()) return std::nullopt;

    return static_cast<unsigned int>(result);
  } catch (const std::exception&) {
    return std::nullopt;
  }
}

std::optional<logging::LogLevel> LoadTesterArguments::ParseLogLevel(const utility::Arguments& arguments) {
  const auto values = arguments.GetValues("log");
  if (!values || values->empty()) return std::nullopt;

  const auto result = values->front();
  if (result == "verbose") return logging::LogLevel::verbose;
  if (result == "debug") return logging::LogLevel::debug;
  if (result == "info") return logging::LogLevel::info;
  if (result == "warning") return logging::LogLevel::warning;
  if (result == "error") return logging::LogLevel::error;
  if (result == "critical") return logging::LogLevel::critical;

  return std::nullopt;
}

}