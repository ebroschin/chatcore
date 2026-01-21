#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace claw::chat::server {

struct JsonNetworkCodec {
  using DiscriminatorType = std::uint64_t;
  using PayloadType = nlohmann::json;

  template<typename TMessage>
  static std::vector<std::byte> Encode(const TMessage& message) { //TODO non-alloc
    const nlohmann::json result {
      {"type_id", TMessage::TypeId},
      {"payload", message},
    };

    std::string serialized_result = result.dump();
    auto bytes_ptr = reinterpret_cast<const std::byte*>(serialized_result.data());
    return {bytes_ptr, bytes_ptr + serialized_result.size()};
  }

  template<typename TMessage>
  static TMessage Decode(const PayloadType& payload) {
    return payload.get<TMessage>();
  }

  static std::optional<std::pair<DiscriminatorType, PayloadType>> DecodePayload(std::span<const std::byte> bytes) {
    if (bytes.empty()) return std::nullopt;

    //non-throwing parse
    nlohmann::json json = nlohmann::json::parse(bytes.begin(), bytes.end(), nullptr, false);
    if (json.is_discarded()) return std::nullopt;
    if (!json.contains("type_id")) return std::nullopt;
    if (!json.contains("payload")) return std::nullopt;

    return std::make_pair(json["type_id"], json["payload"]);
  }
};

}