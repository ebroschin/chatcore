#pragma once

#include <ebroschin/core/system_context.h>
#include <span>
#include <string_view>

namespace ebroschin::chatcore::client {

class ModelSystem;
class SessionSystem;

class ConnectCommand {
public:
  static constexpr std::string_view Token = "connect";
  static constexpr std::string_view Description = "<address> <port> | Connect to the chat server";

  explicit ConnectCommand(const core::SystemContext& ctx) noexcept;
  void Execute(std::span<std::string_view> arguments) const;

private:
  SessionSystem& session_system_;
  ModelSystem& model_system_;
};

}