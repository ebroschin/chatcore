#pragma once

namespace claw::communication {

template<typename TDiscriminator, typename TMessage>
concept NetworkMessage = requires(TMessage message) {
  { TMessage::TypeId } -> std::same_as<TDiscriminator>;
};

template<typename TDiscriminator, typename... TMessages>
concept NetworkMessages =
  (NetworkMessage<TMessages, TDiscriminator> && ...);

template<typename TTcpConnector>
concept NetworkConnector = std::derived_from<TTcpConnector,
  TcpConnector<typename TTcpConnector::ConnectionType, typename TTcpConnector::ParameterType>>
  && std::derived_from<typename TTcpConnector::ConnectionType, TcpConnection>;

template<typename TCodec, typename TMessage>
concept NetworkCodecFor =
requires(typename TCodec::RawType& bytes,
    typename TCodec::PayloadType& payload,
    const TMessage& message)
{
  typename TCodec::RawType;
  typename TCodec::DiscriminatorType;
  typename TCodec::PayloadType;

  { TCodec::template Encode<TMessage>(message) }
  -> std::same_as<typename TCodec::RawType>;

  { TCodec::DecodePayload(bytes) }
  -> std::same_as<std::pair<typename TCodec::DiscriminatorType, typename TCodec::PayloadType>>;

  { TCodec::template Decode<TMessage>(payload) }
  -> std::same_as<TMessage>;
};

template<typename TCodec, typename... TMessages>
concept NetworkCodec =
    (NetworkCodecFor<TCodec, TMessages> && ...);

}