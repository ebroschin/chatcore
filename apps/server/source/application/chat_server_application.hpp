#pragma once

#include "chat_server_arguments.hpp"
#include "connection_event_handler.hpp"

#include <ebroschin/core/application.hpp>
#include <ebroschin/core/synchronization/queued_executor.hpp>

#include <thread>

namespace ebroschin::chatcore::server {

class ChatServerApplication final : public core::Application {
public:
  explicit ChatServerApplication(ChatServerArguments arguments);

  [[nodiscard]] const ChatServerArguments& GetArguments() const noexcept
  { return arguments_; }

  void HandleRpcError(network::ConnectionId connection_id, network::RequestId request_id, const std::string& message) const;

  template <typename TSystem, typename TMessage>
  void RegisterMessageHandler(TSystem* system, void(TSystem::*method)(network::ConnectionId, const TMessage&)) {
    if (system == nullptr) return;
    auto subscription = ctx_.Require<ChatServerTcpSystem>().Subscribe<TMessage>([system, method](const network::NetworkEvent<TMessage>& event) {
      (system->*method)(event.connection_id.value_or(0), event.data); //TODO refactor later, pass NetworkEvent directly
    });

    subscriptions_.emplace_back(std::move(subscription));
  }

protected:
  void PrepareContext() override;
  void OnContextInitialized() override;
  void OnContextDeinitialized() override;
  void HandleTerminate() override;

private:
  ChatServerArguments arguments_;
  core::QueuedExecutor executor_{};
  std::jthread application_thread_{};
  std::unique_ptr<ConnectionEventHandler> connection_event_handler_{};
  std::vector<utility::SignalSubscription> subscriptions_{};
};

}