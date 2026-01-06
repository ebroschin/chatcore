#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace claw::chat::server {

struct TestCodec {
  using RawType = std::string;
  using DiscriminatorType = std::uint64_t;
  using PayloadType = nlohmann::json;

  template<typename TMessage>
  static RawType Encode(const TMessage& message) {
    const nlohmann::json payload = message;
    const nlohmann::json result {
      {"type_id", TMessage::TypeId},
      {"payload", payload},
    };

    return result.dump();//std::string(dumped.begin(), dumped.end());
  }

  template<typename TMessage>
  static TMessage Decode(const PayloadType& payload) {
    return payload.get<TMessage>();
  }

  static std::pair<DiscriminatorType, PayloadType> DecodePayload(const RawType& bytes) {
    std::string text(bytes.begin(), bytes.end());
    nlohmann::json json = nlohmann::json::parse(text);

    return {json["type_id"], json["payload"]};
  }
};

}