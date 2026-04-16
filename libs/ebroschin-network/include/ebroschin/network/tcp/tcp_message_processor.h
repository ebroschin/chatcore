#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

#include "tcp_system_concepts.h"

namespace ebroschin::network::tcp {

template <typename TCodec,
typename TMessageHandler,
typename... TMessages>
requires NetworkCodec<TCodec, TMessages...>
class TcpMessageProcessor {
  using Task = std::function<void()>;

public:
  using MessageHandler = TMessageHandler;

  void ProcessBlocking() {
    std::queue<Task> buffer;

    {
      std::unique_lock lock(mutex_);
      cv_.wait(lock, [&]{ return stopped_ || !tasks_.empty(); });
      if (stopped_) return;

      buffer.swap(tasks_);
    }

    while (!buffer.empty()) {
      auto task = std::move(buffer.front());
      buffer.pop();
      task();
    }
  }

  void Stop() {
    {
      std::lock_guard lock(mutex_);
      stopped_ = true;
    }

    cv_.notify_all();
  }

  void Enqueue(ConnectionId id, std::vector<std::byte> bytes) {
    const auto envelope = TCodec::DecodeEnvelope(bytes);
    if (!envelope) return;

    static const auto message_handler_lookup = CreateMessageHandlerLookup();
    const auto type_id = envelope->first;
    const auto it = message_handler_lookup.find(type_id);
    if (it == message_handler_lookup.end()) return;

    const auto handler = it->second;
    const auto payload = std::move(envelope->second);
    const auto task = [this, handler, id, payload] { handler(this, id, payload); };

    {
      std::lock_guard lock(mutex_);
      if (stopped_) return;

      tasks_.emplace(task);
    }

    cv_.notify_one();
  }

  [[nodiscard]] MessageHandler& GetMessageHandler() noexcept {
    return message_handler_;
  }

private:
  template<typename TMessage>
  static void HandleMessage(TcpMessageProcessor* self, ConnectionId id, const TCodec::PayloadType& payload) {
    if (self == nullptr) return;

    auto message = TCodec::template Decode<TMessage>(payload);
    if (!message) return;

    self->message_handler_.HandleMessage(id, *message);
  }

  static auto CreateMessageHandlerLookup() {
    using FunctionType = void(*)(TcpMessageProcessor*, ConnectionId, const typename TCodec::PayloadType&);
    std::unordered_map<typename TCodec::DiscriminatorType, FunctionType> result;
    ((result[TMessages::TypeId] = &TcpMessageProcessor::HandleMessage<TMessages>), ...);
    return result;
  }

  std::mutex mutex_{};
  std::queue<Task> tasks_{};
  std::condition_variable cv_{};
  std::atomic<bool> stopped_{};

  TMessageHandler message_handler_{};
};

}