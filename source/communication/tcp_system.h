#pragma once

#include "commons.h"
#include "message_handler_registry.h"
#include "tcp_connection.h"
#include "tcp_system_concepts.h"
#include <atomic>
#include <claw/core/system.h>
#include <concepts>
#include <mutex>
#include <shared_mutex>
#include <nlohmann/json.hpp>

namespace claw::communication {

template<NetworkConnector TConnector,
typename TCodec,
typename... TMessages>
requires NetworkCodec<TCodec, TMessages...>
class TcpSystem final: public core::System {
  template<typename T>
  static constexpr bool IsValidMessage = (std::same_as<T, TMessages> || ...);

public:
  explicit TcpSystem(const core::SystemContext& ctx)
    : System(ctx),
    connector_{std::make_unique<TConnector>()}
  {}

  void Connect(const TConnector::ParameterType& parameters, std::function<void(ConnectionID)> callback = nullptr) {
    static std::atomic<ConnectionID> id_counter{1};

    connector_->Connect(parameters, [this, callback]
      (std::shared_ptr<typename TConnector::ConnectionType> connection) {
      auto connection_id = id_counter.fetch_add(1, std::memory_order_relaxed);

      {
        std::unique_lock lock(connections_mutex_);
        connections_.emplace(connection_id, connection);
      }

      connection->Start([this, connection_id](std::span<const std::byte> bytes) {
        ReceiveMessage(connection_id, bytes);
      });

      if (!callback) return;
      callback(connection_id);
    });
  }

  //TODO messages are known at compile time, so do the message handlers.
  // it should be possible to register those at compile time too.
  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void RegisterMessageHandler(std::function<void(ConnectionID id, const TMessage&)> function) {
    message_handler_registry_.template Register<TMessage>(function);
  }

  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void Send(ConnectionID id, const TMessage& message) {
    auto bytes = TCodec::template Encode<TMessage>(message);
    std::shared_ptr<TcpConnection> connection;

    {
      std::shared_lock lock(connections_mutex_);
      auto it = connections_.find(id);
      if (it == connections_.end()) return;

      connection = it->second;
    }

    connection->SendBytes(bytes);
  }

  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void Broadcast(const TMessage& message) {
    auto bytes = TCodec::template Encode<TMessage>(message);
    std::vector<std::shared_ptr<TcpConnection>> connections; //TODO allocates with every broadcast

    {
      std::shared_lock lock(connections_mutex_);
      connections.reserve(connections_.size());
      for (const auto &connection : connections_ | std::views::values) {
        connections.push_back(connection);
      }
    }

    for (const auto& connection : connections) {
      connection->SendBytes(bytes);
    }
  }

private:
  //TODO thread safety
  void RemoveConnection(ConnectionID id) {
    auto it = connections_.find(id);
    if (it == connections_.end()) return; //TODO error logging

    connections_.erase(it);
  }

  void ReceiveMessage(ConnectionID id, std::span<const std::byte> bytes) {
    auto pair = TCodec::DecodePayload(bytes);
    static const auto message_handler_lookup = CreateMessageHandlerLookup();

    auto type_id = pair.first;
    auto it = message_handler_lookup.find(type_id);
    if (it == message_handler_lookup.end()) return; //TODO error logging

    it->second(this, id, pair.second);
  }

  template<typename TMessage>
  static void HandleMessage(TcpSystem* self, ConnectionID id, const TCodec::PayloadType& payload) {
    auto message = TCodec::template Decode<TMessage>(payload);
    self->message_handler_registry_.HandleMessage(id, message);
  }

  static auto CreateMessageHandlerLookup() {
    using FunctionType = void(*)(TcpSystem*, ConnectionID, const typename TCodec::PayloadType&);
    std::unordered_map<typename TCodec::DiscriminatorType, FunctionType> result;
    ((result[TMessages::TypeId] = &TcpSystem::HandleMessage<TMessages>), ...);
    return result;
  }

  std::unique_ptr<TConnector> connector_;
  std::shared_mutex connections_mutex_{};
  std::unordered_map<ConnectionID, std::shared_ptr<TcpConnection>> connections_{};
  MessageHandlerRegistry<TMessages...> message_handler_registry_{};
};

}