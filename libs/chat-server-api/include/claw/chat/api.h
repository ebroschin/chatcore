#pragma once

#include <claw/network/rpc/commons.h>

#include <tuple>
#include <cstdint>
#include <string>
#include <vector>

namespace claw::chat::api {

using PersistenceId = std::uint32_t; //TODO strong type via struct

struct ChatMessage {
  PersistenceId id;
  PersistenceId channel_id;
  PersistenceId user_id;
  std::string content;
};

struct ChatChannel {
  PersistenceId id;
  std::string name;
};

struct User {
  PersistenceId id;
  std::string name;
};

struct PrintMessage {
  static constexpr std::uint64_t TypeId = 99;

  std::string value;
};

struct ErrorResponseMessage {
  static constexpr std::uint64_t TypeId = 100;

  network::RequestId request_id;
  std::string value;
};

struct ErrorMessage {
  static constexpr std::uint64_t TypeId = 101;

  std::string value;
};

struct ShutdownMessage {
  static constexpr std::uint64_t TypeId = 102;
};

struct ReceiveChatMessage {
  static constexpr std::uint64_t TypeId = 103;

  PersistenceId user_id;
  PersistenceId channel_id;
  std::string content;
};

struct WriteChatMessage {
  static constexpr std::uint64_t TypeId = 104;

  std::string content;
};

struct GetChatsRequestMessage {
  static constexpr std::uint64_t TypeId = 105;

  network::RequestId request_id;
  PersistenceId channel_id;
  std::uint32_t limit;
};

struct GetChatsResponseMessage {
  static constexpr std::uint64_t TypeId = 106;

  network::RequestId request_id;
  PersistenceId channel_id;
  std::vector<ChatMessage> messages;
};

struct CreateChannelRequestMessage {
  static constexpr std::uint64_t TypeId = 107;

  network::RequestId request_id;
  std::string name;
};

struct CreateChannelResponseMessage {
  static constexpr std::uint64_t TypeId = 108;

  network::RequestId request_id;
  ChatChannel channel;
};

struct CreateUserRequestMessage {
  static constexpr std::uint64_t TypeId = 109;

  network::RequestId request_id;
  std::string name;
  std::string password;
};

struct CreateUserResponseMessage {
  static constexpr std::uint64_t TypeId = 110;

  network::RequestId request_id;
  User user;
};

struct GetUsersRequestMessage {
  static constexpr std::uint64_t TypeId = 111;

  network::RequestId request_id;
  std::vector<PersistenceId> user_ids;
};

struct GetUsersResponseMessage {
  static constexpr std::uint64_t TypeId = 112;

  network::RequestId request_id;
  std::vector<User> users;
};

struct AuthenticateUserRequestMessage {
  static constexpr std::uint64_t TypeId = 113;

  network::RequestId request_id;
  std::string name;
  std::string password;
};

struct AuthenticateUserResponseMessage {
  static constexpr std::uint64_t TypeId = 114;

  network::RequestId request_id;
  User user;
};

struct JoinChatChannelRequestMessage {
  static constexpr std::uint64_t TypeId = 115;

  network::RequestId request_id;
  PersistenceId channel_id;
};

struct JoinChatChannelResponseMessage {
  static constexpr std::uint64_t TypeId = 116;

  network::RequestId request_id;
  PersistenceId channel_id;
};

struct GetChatChannelsRequestMessage {
  static constexpr std::uint64_t TypeId = 117;

  network::RequestId request_id;
};

struct GetChatChannelsResponseMessage {
  static constexpr std::uint64_t TypeId = 118;

  network::RequestId request_id;
  std::vector<ChatChannel> channels;
};

struct LogoutRequestMessage {
  static constexpr std::uint64_t TypeId = 119;

  network::RequestId request_id;
};

struct LogoutResponseMessage {
  static constexpr std::uint64_t TypeId = 120;

  network::RequestId request_id;
};

struct UserLogoutEventMessage {
  static constexpr std::uint64_t TypeId = 121;

  PersistenceId user_id;
};

struct UserLoginEventMessage {
  static constexpr std::uint64_t TypeId = 122;

  PersistenceId user_id;
};

struct ChannelLeaveEventMessage {
  static constexpr std::uint64_t TypeId = 123;

  PersistenceId channel_id;
  PersistenceId user_id;
};

struct ChannelJoinEventMessage {
  static constexpr std::uint64_t TypeId = 124;

  PersistenceId channel_id;
  PersistenceId user_id;
};

struct ChannelCreateEventMessage {
  static constexpr std::uint64_t TypeId = 125;

  ChatChannel channel;
  PersistenceId user_id;
};

using MessageTypes = std::tuple<
  PrintMessage,
  ErrorMessage,
  ErrorResponseMessage,
  ShutdownMessage,
  ReceiveChatMessage,
  WriteChatMessage,
  GetChatsRequestMessage,
  GetChatsResponseMessage,
  CreateChannelRequestMessage,
  CreateChannelResponseMessage,
  CreateUserRequestMessage,
  CreateUserResponseMessage,
  GetUsersRequestMessage,
  GetUsersResponseMessage,
  AuthenticateUserRequestMessage,
  AuthenticateUserResponseMessage,
  JoinChatChannelRequestMessage,
  JoinChatChannelResponseMessage,
  GetChatChannelsRequestMessage,
  GetChatChannelsResponseMessage,
  LogoutRequestMessage,
  LogoutResponseMessage,
  UserLogoutEventMessage,
  UserLoginEventMessage,
  ChannelLeaveEventMessage,
  ChannelJoinEventMessage,
  ChannelCreateEventMessage
>;

}

template <>
struct claw::network::rpc::RpcCall<claw::chat::api::AuthenticateUserRequestMessage> {
  using Response = chat::api::AuthenticateUserResponseMessage;
  using Error = chat::api::ErrorResponseMessage;
};

template <>
struct claw::network::rpc::RpcCall<claw::chat::api::JoinChatChannelRequestMessage> {
  using Response = chat::api::JoinChatChannelResponseMessage;
  using Error = chat::api::ErrorResponseMessage;
};

template <>
struct claw::network::rpc::RpcCall<claw::chat::api::GetChatsRequestMessage> {
  using Response = chat::api::GetChatsResponseMessage;
  using Error = chat::api::ErrorResponseMessage;
};

template <>
struct claw::network::rpc::RpcCall<claw::chat::api::GetChatChannelsRequestMessage> {
  using Response = chat::api::GetChatChannelsResponseMessage;
  using Error = chat::api::ErrorResponseMessage;
};

template <>
struct claw::network::rpc::RpcCall<claw::chat::api::CreateUserRequestMessage> {
  using Response = chat::api::CreateUserResponseMessage;
  using Error = chat::api::ErrorResponseMessage;
};

template <>
struct claw::network::rpc::RpcCall<claw::chat::api::CreateChannelRequestMessage> {
  using Response = chat::api::CreateChannelResponseMessage;
  using Error = chat::api::ErrorResponseMessage;
};

template <>
struct claw::network::rpc::RpcCall<claw::chat::api::GetUsersRequestMessage> {
  using Response = chat::api::GetUsersResponseMessage;
  using Error = chat::api::ErrorResponseMessage;
};

template <>
struct claw::network::rpc::RpcCall<claw::chat::api::LogoutRequestMessage> {
  using Response = chat::api::LogoutResponseMessage;
  using Error = chat::api::ErrorResponseMessage;
};