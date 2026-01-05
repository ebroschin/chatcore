#pragma once

namespace claw::communication {

template<typename TSerializer, typename TMessage>
concept NetworkSerializerFor =
requires(const typename TSerializer::RawType& data, const TMessage& message) {
    typename TSerializer::RawType;

    { TSerializer::template Serialize<TMessage>(message) }
    -> std::same_as<typename TSerializer::RawType>;

    { TSerializer::template Deserialize<TMessage>(data) }
    -> std::same_as<TMessage>;
};

template<typename TSerializer, typename... TMessages>
concept NetworkSerializer =
    (NetworkSerializerFor<TSerializer, TMessages> && ...);

}