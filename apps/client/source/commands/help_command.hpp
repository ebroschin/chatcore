#pragma once

#include <ebroschin/core/system_context.hpp>

#include "../model/model_system.hpp"
#include <span>
#include <string_view>

namespace ebroschin::chatcore::client {

class HelpCommand {
public:
  static constexpr std::string_view Token = "help";
  static constexpr std::string_view Description = "| Display usage information for each command";

  explicit HelpCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments);

private:
  ModelSystem& model_system_;
};

}