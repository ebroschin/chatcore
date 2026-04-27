#pragma once

#include <ebroschin/core/system_context.hpp>
#include <span>
#include <string_view>

namespace ebroschin::chatcore::client {

class ApplicationSystem;

class ExitCommand {
public:
  static constexpr std::string_view Token = "exit";
  static constexpr std::string_view Description = "| Quit the client application";

  explicit ExitCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  ApplicationSystem& application_system_;
};

}
