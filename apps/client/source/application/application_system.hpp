#pragma once

#include "client_tcp_system.hpp"

#include <ebroschin/core/application.hpp>
#include <ebroschin/core/system.hpp>

#include <thread>

namespace ebroschin::chatcore::client {

class ApplicationSystem final : public core::System {
public:
  explicit ApplicationSystem(const core::SystemContext& ctx, core::Application& app) noexcept;

  void Initialize() override;
  void Deinitialize() override;
  void Quit() const noexcept;

  [[nodiscard]] ClientTcpSystem::MessageHandler& GetMessageHandler() const noexcept
  { return message_handler_; }

private:
  core::Application& app_;
  ClientTcpSystem& tcp_system_;
  ClientTcpSystem::MessageHandler& message_handler_;

  std::jthread application_thread_{};
};

}