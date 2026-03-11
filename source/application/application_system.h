#pragma once

#include <claw/core/system.h>
#include <thread>
#include <functional>
#include "commons.h"

namespace claw::chat::server {

class ChatServerApplication;

class ApplicationSystem final : public core::System {
public:
  explicit ApplicationSystem(const core::SystemContext& ctx, ChatServerApplication& app);

  void Initialize() override;
  void Deinitialize() override;

  void Shutdown() const noexcept;

  template<typename TMessage>
  void RegisterMessageHandler(std::function<void(network::ConnectionId id, const TMessage&)> function) {
    message_handler_.Register<TMessage>(function);
  }

private:
  ChatServerApplication& app_;
  ChatServerTcpSystem& tcp_system_;
  ChatServerTcpSystem::MessageHandler& message_handler_;
  std::jthread application_thread_{};
};

}