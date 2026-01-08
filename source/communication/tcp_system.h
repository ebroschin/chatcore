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
    static std::atomic<ConnectionID> id_counter{0};

    connector_->Connect(parameters, [this, callback](std::shared_ptr<typename TConnector::ConnectionType> ptr) {
      auto connection_id = ++id_counter;
      {
        std::unique_lock lock(connections_mutex_);
        connections_.emplace(connection_id, ptr);
      }

      ptr->Start([this, connection_id](std::span<const std::byte> bytes) {
        ReceiveMessage(connection_id, bytes);
      });

      //ptr->Start(std::bind(&TcpSystem::ReceiveMessage, this, std::placeholders::_1));

      if (!callback) return;
      callback(connection_id);
    });
  }

  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void RegisterMessageHandler(std::function<void(ConnectionID id, const TMessage&)> function) {
    message_handler_registry_.template Register<TMessage>(function);
  }

  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void SendMessage(ConnectionID id, const TMessage& message) {
    auto bytes = TCodec::template Encode<TMessage>(message);

    std::shared_ptr<TcpConnection> connection;
    {
      std::shared_lock lock(connections_mutex_);
      auto it = connections_.find(id);
      if (it == connections_.end()) return;

      connection = it->second;
    }

    connection->SendMessage(bytes);
  }

  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void BroadcastMessage(const TMessage& message) {
    // 1. Encode ONCE into a shared buffer
    //auto shared_payload = std::make_shared<std::vector<std::byte>>();
    auto bytes = TCodec::template Encode<TMessage>(message);
    // Optimization: Reserve/Resize buffer if size known

    std::vector<std::shared_ptr<TcpConnection>> snapshot;

    // 2. Fast Snapshot
    {
      std::shared_lock lock(connections_mutex_);
      snapshot.reserve(connections_.size());
      for (const auto &conn : connections_ | std::views::values) {
        snapshot.push_back(conn);
      }
    } // Lock released immediately

    // 3. Parallel/Async Dispatch
    for (const auto& conn : snapshot) {
      conn->SendMessage(bytes);
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

  //TODO implement connection pooling
  std::shared_mutex connections_mutex_;
  std::unordered_map<ConnectionID, std::shared_ptr<TcpConnection>> connections_;
  MessageHandlerRegistry<TMessages...> message_handler_registry_{};
};

}