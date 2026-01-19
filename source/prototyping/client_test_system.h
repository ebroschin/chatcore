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
  void Deinitialize() override;

  void HandleLine(const std::string& line);

  [[nodiscard]] auto ConnectionID() { return connection_id_; }

private:
  void ParseCommand(std::string_view view, std::vector<std::string>& result);

  chat::client::ChatInputSystem& chat_input_system_;
  chat::server::ChatClientTcpSystem& tcp_system_;
  network::ConnectionId connection_id_{};
  std::unique_ptr<chat::api::User> user_{};

  bool running_{true};
  std::thread worker_{};
};

}