#pragma once

#include "../codec/test_codec.h"
#include "message_handler_registry.h"
#include "tcp_connection.h"
#include "tcp_connector.h"
#include "tcp_system_concepts.h"
#include <claw/core/system.h>
#include <concepts>
#include <nlohmann/json.hpp>

namespace claw::communication {

struct TestMessage {
  static constexpr std::uint64_t TypeId = 100;
  std::string value;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TestMessage, value)

struct OtherMessage {
  static constexpr std::uint64_t TypeId = 101;
  int value;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(OtherMessage, value)

struct AnotherMessage {
  static constexpr std::uint64_t TypeId = 102;
  float value;
  float value2;
  float value3;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnotherMessage, value, value2, value3)

struct UnknownMessage {
  std::string value;
};

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

  void Update() override {
    if (connection_ == nullptr) return;
    if (!connection_->HasData()) return;

    std::string bytes = connection_->ReadMessage();

    auto pair = TCodec::DecodePayload(bytes);
    static const auto message_handler_lookup = CreateMessageHandlerLookup();

    auto type_id = pair.first;
    if (auto it = message_handler_lookup.find(type_id); it != message_handler_lookup.end()) {
      it->second(this, pair.second);
    }
  }

  void Connect(const TConnector::ParameterType& parameters) {
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
  void SendMessage(const TMessage& message) {
    std::string serialized_message = TCodec::template Encode<TMessage>(message);
    connection_->SendMessage(serialized_message);
  }

  //TODO void BroadcastMessage(const std::string& message);

private:
  template<typename TMessage>
  requires IsValidMessage<TMessage>
  void HandleMessage(const TCodec::PayloadType& payload) {
    auto message = TCodec::template Decode<TMessage>(payload);
    message_handler_registry_.HandleMessage(message);
  }

  template<typename TMessage>
  static constexpr void HandleMessageInternal(TcpSystem* self, const TCodec::PayloadType& payload) {
    self->HandleMessage<TMessage>(payload);
  }

  static auto CreateMessageHandlerLookup() {
    using function_type = void(*)(TcpSystem*, const typename TCodec::PayloadType&);
    std::unordered_map<typename TCodec::DiscriminatorType, function_type> result;
    ((result[TMessages::TypeId] = &TcpSystem::HandleMessageInternal<TMessages>), ...);
    return result;
  }

  std::unique_ptr<TConnector> connector_;

  //TODO refactor to multi-connection later
  std::unique_ptr<TcpConnection> connection_;
  MessageHandlerRegistry<TMessages...> message_handler_registry_{};
};

}