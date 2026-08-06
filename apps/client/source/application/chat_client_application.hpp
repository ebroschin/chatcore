#pragma once

#include "client_tcp_system.hpp"

#include <ebroschin/core/application.hpp>
#include <ebroschin/core/synchronization/queued_executor.hpp>
#include <ebroschin/logging/log.hpp>

#include <thread>
#include <utility>

namespace ebroschin::chatcore::client {

class ChatClientApplication final : public core::Application {
public:
  void PrepareContext() override;
  void OnContextInitialized() override;
  void OnContextDeinitialized() override;
  void HandleTerminate() override;

  template <typename TInstance, typename TMessage>
  void RegisterEventMessageHandler(TInstance* instance, void(TInstance::*method)(const TMessage&) const) {
    RegisterEventMessageHandlerImpl<TMessage>(instance, method);
  }

  template <typename TInstance, typename TMessage>
  void RegisterEventMessageHandler(TInstance* instance, void(TInstance::*method)(const TMessage&)) {
    RegisterEventMessageHandlerImpl<TMessage>(instance, method);
  }

  template <typename RpcCall>
  void RegisterDefaultErrorHandler(RpcCall&& rpcCall) const {
    rpcCall.OnError([](const api::ErrorResponseMessage& response) {
      logging::Log::Error() << response.value;
    });
  }

  template <typename RpcCall>
  void RegisterDefaultTimeoutHandler(RpcCall&& rpcCall, const std::string& message) const {
    using namespace std::chrono_literals;

    rpcCall.SetTimeoutDuration(5s);
    rpcCall.OnTimeout([message] {
      logging::Log::Error() << message;
    });
  }

private:
  template <typename TMessage, typename TInstance, typename TMethod>
  void RegisterEventMessageHandlerImpl(TInstance* instance, TMethod method) {
    auto& signals = ctx_.Require<ClientTcpSystem>().GetMessageHandler(); //TODO store ref
    auto subscription = signals.Subscribe<TMessage>([instance, method]
    (const network::NetworkEvent<TMessage>& event)
    {
      (instance->*method)(event.data);
    });

    subscriptions_.emplace_back(std::move(subscription));
  }

  core::QueuedExecutor executor_{};
  std::jthread application_thread_{};
  std::vector<utility::SignalSubscription> subscriptions_{};
};

}