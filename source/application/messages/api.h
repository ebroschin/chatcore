#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace claw::chat::api {

struct ChatMessage {
  std::uint32_t user_id;
  std::string content;
};

struct WriteChatMessage {
  static constexpr std::uint64_t TypeId = 104;

  std::uint32_t channel_id;
  ChatMessage message;
};

struct GetChatsRequestMessage {
  static constexpr std::uint64_t TypeId = 105;

  std::uint32_t channel_id;
};

struct GetChatsResponseMessage {
  static constexpr std::uint64_t TypeId = 106;

  std::uint32_t channel_id;
  std::vector<ChatMessage> messages;
};

struct CreateChannelMessage {
  static constexpr std::uint64_t TypeId = 107;

  std::string name;
};

}