#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../commons.h"

namespace claw::chat::api {

struct ChatMessage {
  PersistenceID user_id;
  std::string content;
};

struct User {
  PersistenceID id;
  std::string name;
};

struct PrintMessage {
  static constexpr std::uint64_t TypeId = 100;
  std::string value;
};

struct WriteChatMessage {
  static constexpr std::uint64_t TypeId = 104;

  PersistenceID channel_id;
  ChatMessage message;
};

struct GetChatsRequestMessage {
  static constexpr std::uint64_t TypeId = 105;

  PersistenceID channel_id;
};

struct GetChatsResponseMessage {
  static constexpr std::uint64_t TypeId = 106;

  PersistenceID channel_id;
  std::vector<ChatMessage> messages;
};

struct CreateChannelMessage {
  static constexpr std::uint64_t TypeId = 107;

  std::string name;
};

struct CreateUserMessage {
  static constexpr std::uint64_t TypeId = 108;

  std::string name;
  std::string password;
};

struct GetUserRequestMessage {
  static constexpr std::uint64_t TypeId = 109;

  PersistenceID user_id;
};

struct GetUserResponseMessage {
  static constexpr std::uint64_t TypeId = 110;

  User user;
};

struct AuthenticateUserRequestMessage {
  static constexpr std::uint64_t TypeId = 111;

  std::string name;
  std::string password;
};

struct AuthenticateUserResponseMessage {
  static constexpr std::uint64_t TypeId = 112;

  User user;
};

}