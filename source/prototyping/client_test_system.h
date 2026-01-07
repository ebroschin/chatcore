#pragma once

#include "../application/chat_tcp_system.h"
#include <claw/core/system.h>

namespace claw::chat::client {
class ChatInputSystem;
}

namespace claw::prototyping {

class ClientTestSystem final : public core::System {
public:
  explicit ClientTestSystem(const core::SystemContext& ctx);

  void Initialize() override;
  void Update() override;

  [[nodiscard]] auto ConnectionID() { return connection_id_; }

private:
  chat::client::ChatInputSystem& chat_input_system_;
  chat::server::ChatClientTcpSystem& tcp_system_;
  communication::ConnectionID connection_id_{};
};

}