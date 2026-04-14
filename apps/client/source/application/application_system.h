#pragma once

#include <thread>

#include "claw/core/system.h"
#include "client_tcp_system.h"

namespace claw::chat::client {

class ApplicationSystem final : public core::System {
public:
  explicit ApplicationSystem(const core::SystemContext& ctx) noexcept;

  void Initialize() override;
  void Deinitialize() override;

  [[nodiscard]] ClientTcpSystem::MessageHandler& GetMessageHandler() const noexcept
  { return message_handler_; }

private:
  ClientTcpSystem& tcp_system_;
  ClientTcpSystem::MessageHandler& message_handler_;

  std::jthread application_thread_{};
};

}