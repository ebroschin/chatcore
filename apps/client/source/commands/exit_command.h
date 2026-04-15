#pragma once

#include <claw/core/system_context.h>
#include <span>
#include <string_view>

namespace claw::chat::client {

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
