#pragma once

#include <ebroschin/core/system_context.hpp>

#include <span>
#include <string_view>

namespace ebroschin::chatcore::client {

class SessionSystem;

class JoinChannelCommand {
public:
  static constexpr std::string_view Token = "join";
  static constexpr std::string_view Description =
    "<channel_id> | Join a chat channel and receive its latest messages";

  explicit JoinChannelCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  SessionSystem& session_system_;
};

}