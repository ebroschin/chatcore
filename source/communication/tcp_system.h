#pragma once

#include "commons.h"
#include "message_handler_registry.h"
#include "tcp_connection.h"
#include "tcp_system_concepts.h"
#include <claw/core/system.h>
#include <concepts>
#include <mutex>
#include <claw/utility/task_thread.h>
#include <ranges>

namespace claw::communication {

template<NetworkConnector TConnector,
typename TCodec,
typename... TMessages>
requires NetworkCodec<TCodec, TMessages...>
class TcpSystem final: public core::System {
  template<typename T>
  static constexpr bool IsValidMessage = (std::same_as<T, TMessages> || ...);

  using ConnectionCallback = std::function<void(ConnectionID)>;

public:
  explicit TcpSystem(const core::SystemContext& ctx)
    : System(ctx),
    connector_{std::make_unique<TConnector>()}
  {}

  void Connect(const TConnector::ParameterType& parameters, ConnectionCallback callback = nullptr) {
    connector_->Connect(parameters, [this, callback]
      (std::shared_ptr<typename TConnector::ConnectionType> connection)
    {
        CreateConnection(std::move(connection), callback);
    });
  }

  //TODO messages are known at compile time, so are the message handlers.
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

    task_thread.Post([this, id, bytes = std::move(bytes)]() {
      auto it = connections_.find(id);
      if (it == connections_.end()) return;

      it->second->SendBytes(bytes);
    });
  }

  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void Broadcast(const TMessage& message) {
    auto bytes = TCodec::template Encode<TMessage>(message);

    task_thread.Post([this, bytes = std::move(bytes)]() {
      for (const auto &connection : connections_ | std::views::values) {
        connection->SendBytes(bytes);
      }
    });
  }

private:
  void CreateConnection(std::shared_ptr<typename TConnector::ConnectionType>&& connection, ConnectionCallback callback) {
    task_thread.Post([this,
      connection = std::move(connection),
      callback = std::move(callback)]()
    {
      auto connection_id = id_counter_++;
      connections_.emplace(connection_id, connection);

      const auto receiver_callback = [this, connection_id](std::span<const std::byte> bytes) {
        ReceiveMessage(connection_id, bytes);
      };

      const auto disconnect_callback = [this, connection_id]() {
        RemoveConnection(connection_id);
      };

      connection->Start(receiver_callback, disconnect_callback);

      if (!callback) return;
      callback(connection_id);
    });
  }

  void RemoveConnection(ConnectionID id) {
    task_thread.Post([this, id]() {
      auto it = connections_.find(id);
      if (it == connections_.end()) return;

      connections_.erase(it);
    });
  }

  void ReceiveMessage(ConnectionID id, std::span<const std::byte> bytes) {
    auto payload = TCodec::DecodePayload(bytes);
    if (!payload) return;

    static const auto message_handler_lookup = CreateMessageHandlerLookup();
    task_thread.Post([this, id, payload = std::move(payload)]() {
      auto type_id = payload->first;
      auto it = message_handler_lookup.find(type_id);
      if (it == message_handler_lookup.end()) return; //TODO error logging

      it->second(this, id, payload->second);
    });
  }

  template<typename TMessage>
  static void HandleMessage(TcpSystem* self, ConnectionID id, const TCodec::PayloadType& payload) {
    if (self == nullptr) return;

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
  std::unordered_map<ConnectionID, std::shared_ptr<TcpConnection>> connections_{};
  MessageHandlerRegistry<TMessages...> message_handler_registry_{};
  utility::TaskThread task_thread{};
  ConnectionID id_counter_{1};
};

}