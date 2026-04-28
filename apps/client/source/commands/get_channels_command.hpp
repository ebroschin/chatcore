#pragma once

#include <ebroschin/core/system_context.hpp>

#include <span>
#include <string_view>

namespace ebroschin::chatcore::client {

class SessionSystem;

class GetChannelsCommand {
public:
  static constexpr std::string_view Token = "channels";
  static constexpr std::string_view Description = "| Print all channels and their ids";

  explicit GetChannelsCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  SessionSystem& session_system_;
};

}