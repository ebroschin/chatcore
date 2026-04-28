#pragma once

#include "chat_tcp_system.hpp"
#include "connection_event_handler.hpp"

#include <ebroschin/core/system.hpp>

#include <memory>
#include <thread>

namespace ebroschin::chatcore::server {

class ChatServerApplication;

class ApplicationSystem final : public core::System {
public:
  explicit ApplicationSystem(const core::SystemContext& ctx, ChatServerApplication& app) noexcept;

  void Initialize() override;
  void Deinitialize() override;

  void Shutdown() const noexcept;
  void HandleRpcError(network::ConnectionId connection_id, network::RequestId request_id, const std::string& message) const;

  template <typename TSystem, typename TMessage>
  void RegisterMessageHandler(TSystem* system, void(TSystem::*method)(network::ConnectionId, const TMessage&)) {
    if (system == nullptr) return;
    message_handler_.Register<TMessage>([system, method](network::ConnectionId id, const TMessage& message) {
      (system->*method)(id, message);
    });
  }

private:
  ChatServerApplication& app_;
  ChatServerTcpSystem& tcp_system_;
  ChatServerTcpSystem::MessageHandler& message_handler_;
  std::jthread application_thread_{};
  std::unique_ptr<ConnectionEventHandler> connection_event_handler_{};
};

}