#pragma once

#include "message_handler_registry.h"
#include <claw/core/system.h>
#include <concepts>
#include "tcp_connection.h"
#include "tcp_connector.h"
#include "network_serializer.h"
#include "../utility/variadic.h"

namespace claw::communication {

struct TestMessage {
  std::string value;
};

struct OtherMessage {
  int value;
};

struct AnotherMessage {
  float value;
  float value2;
  float value3;
};

struct UnknownMessage {
  std::string value;
};

struct TestSerializer {
  using RawType = std::string;

  template<typename TMessage>
  static RawType Serialize(const TMessage& message);

  template<typename TMessage>
  static TMessage Deserialize(const RawType& raw);
};

//TODO specifications for client/server are equivalent
// the only real difference is how the connection gets established
// after the handshake, both server and client are peers
// connector (and its connection type) are passed via template parameters

template<typename TTcpConnector,
typename TSerializer,
typename... TMessages>
requires std::derived_from<TTcpConnector,
  TcpConnector<typename TTcpConnector::ConnectionType, typename TTcpConnector::ParameterType>>
  && std::derived_from<typename TTcpConnector::ConnectionType, TcpConnection>
  && NetworkSerializer<TSerializer, TMessages...>
class TcpSystem final: public core::System {
  template<typename T>
  static constexpr bool IsValidMessage = (std::same_as<T, TMessages> || ...);

public:
  using ParameterType = TTcpConnector::ParameterType;

  explicit TcpSystem(const core::SystemContext& ctx)
    : System(ctx),
    connector_{std::make_unique<TTcpConnector>()}
  {}

  void Update() override {
    if (connection_ == nullptr) return;
    if (!connection_->HasData()) return;

    std::string bytes = connection_->ReadMessage();

    int discriminator = 0;
    static auto message_handler_lookup = CreateMessageHandlerLookup();
    message_handler_lookup[discriminator](this, bytes);
  }

  void Connect(const ParameterType& parameters) {
    //TODO support multi connections
    if (connection_ != nullptr && connection_->IsOpen()) return;
    connection_ = connector_->Connect(parameters);
  }

  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void RegisterMessageHandler(std::function<void(const TMessage&)> function) {
    message_handler_registry_.template Register<TMessage>(function);
  }

  //TODO connection id
  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void SendMessage(const TMessage&& message) {
    std::string serialized_message = TSerializer::template Serialize<TMessage>(message);
    connection_->SendMessage(serialized_message);
  }

  //TODO void BroadcastMessage(const std::string& message);

private:
  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void HandleMessage(const std::string& bytes) {
    auto message = TSerializer::template Deserialize<TMessage>(bytes);
    message_handler_registry_.HandleMessage(message);
  }

  template<typename TMessage>
  static void HandleMessageInternal(TcpSystem* self, const std::string& bytes) {
    self->HandleMessage<TMessage>(bytes);
  }

  static auto CreateMessageHandlerLookup() {
    using function_type = void(*)(TcpSystem*, const std::string&);
    std::array<function_type, sizeof...(TMessages)> result;
    ((result[utility::IndexOf<TMessages, TMessages...>()] = &TcpSystem::HandleMessageInternal<TMessages>), ...);
    return result;
  }

  std::unique_ptr<TTcpConnector> connector_;

  //TODO refactor to multi-connection later
  std::unique_ptr<TcpConnection> connection_;
  MessageHandlerRegistry<TMessages...> message_handler_registry_{};
};

}