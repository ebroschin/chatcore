#include "client_test_system.h"

#include <claw/network/tcp/tcp_system.h>
#include <claw/chat/api.h>
#include <claw/core/system_context.h>

#include "../application/chat_tcp_system.h"
#include "chat_input_system.h"

#include <iostream>

namespace claw::prototyping {

ClientTestSystem::ClientTestSystem(const core::SystemContext& ctx):
System(ctx),
chat_input_system_{*ctx.Get<chat::client::ChatInputSystem>()},
tcp_system_{*ctx.Get<chat::server::ChatClientTcpSystem>()} {
  tcp_system_.RegisterMessageHandler<chat::api::PrintMessage>([&](network::ConnectionId, const chat::api::PrintMessage& message) {
    std::cout << "[server::print] " << message.value << std::endl;
  });

  tcp_system_.RegisterMessageHandler<chat::api::ErrorMessage>([&](network::ConnectionId, const chat::api::ErrorMessage& message) {
    std::cout << "[server::error] " << message.value << std::endl;
  });

  tcp_system_.RegisterMessageHandler<chat::api::GetChatsResponseMessage>([&](network::ConnectionId, const chat::api::GetChatsResponseMessage& message) {
    std::cout << "received chats: " << std::endl;
    for (const auto& chat_message : message.messages) {
      std::cout << chat_message.user_id << ": " << chat_message.content << std::endl;
    }
  });

  tcp_system_.RegisterMessageHandler<chat::api::AuthenticateUserResponseMessage>([&](network::ConnectionId, const chat::api::AuthenticateUserResponseMessage& message) {
    std::cout << "login successful" << std::endl;
    user_ = std::make_unique<chat::api::User>(message.user);
  });
}

void ClientTestSystem::Initialize() {
  auto* processor = tcp_system_.CreateMessageProcessor();
  worker_ = std::thread{[this, processor]() {
    while (running_) {
      processor->ProcessBlocking();
    }
  }};
}

void ClientTestSystem::Deinitialize() {
  running_ = false;
  if (!worker_.joinable()) return;
  worker_.join();
}

void ClientTestSystem::HandleLine(const std::string& line) {
  static std::vector<std::string> parameter_buffer;

  if (line.starts_with("connect")) {
    ParseCommand(line, parameter_buffer);
    if (parameter_buffer.size() <= 1) return;

    tcp_system_.Connect({parameter_buffer[1], "1338"}, [this](network::ConnectionId id) {
      connection_id_ = id;
      std::cout << "connection established: " << id << std::endl;
    });

    return;
  }

  if (connection_id_ <= 0) return;

  if (line.starts_with("write")) {
    const std::string message = line.substr(std::strlen("write"));
    tcp_system_.Send<chat::api::WriteChatMessage>(connection_id_, {message});
    return;
  }

  if (line.starts_with("get")) {
    const std::string channel_id = line.substr(std::strlen("get"));
    const auto channel_id_param = static_cast<chat::api::PersistenceId>(std::stoul(channel_id));
    tcp_system_.Send<chat::api::GetChatsRequestMessage>(connection_id_, {channel_id_param});
    return;
  }

  if (line.starts_with("create")) {
    const std::string name = line.substr(std::strlen("create") + 1);
    tcp_system_.Send<chat::api::CreateChannelRequestMessage>(connection_id_, {name});
    return;
  }

  if (line.starts_with("adduser")) {
    ParseCommand(line, parameter_buffer);
    if (parameter_buffer.size() <= 2) return;

    const auto& name = parameter_buffer[1];
    const auto& password = parameter_buffer[2];
    tcp_system_.Send<chat::api::CreateUserRequestMessage>(connection_id_, {name, password});
    return;
  }

  if (line.starts_with("login")) {
    ParseCommand(line, parameter_buffer);
    if (parameter_buffer.size() <= 2) return;

    const auto& name = parameter_buffer[1];
    const auto& password = parameter_buffer[2];
    tcp_system_.Send<chat::api::AuthenticateUserRequestMessage>(connection_id_, {name, password});
    return;
  }

  if (line.starts_with("join")) {
    ParseCommand(line, parameter_buffer);
    if (parameter_buffer.size() <= 1) return;

    auto channel_id = static_cast<chat::api::PersistenceId>(std::stoul(parameter_buffer[1]));
    tcp_system_.Send<chat::api::JoinChatChannelRequestMessage>(connection_id_, {channel_id});
    return;
  }

  if (user_ == nullptr) return;
  tcp_system_.Send<chat::api::WriteChatMessage>(connection_id_, {line});
}

void ClientTestSystem::ParseCommand(std::string_view view, std::vector<std::string>& result) {
  result.clear();

  for (auto split : view | std::views::split(':')) {
    result.emplace_back(split.begin(), split.end());
  }
}

}