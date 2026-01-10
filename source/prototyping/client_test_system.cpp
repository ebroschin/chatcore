#include "client_test_system.h"

#include "../communication/tcp_system.h"
#include "../application/chat_tcp_system.h"
#include <claw/core/system_context.h>
#include "chat_input_system.h"
#include "../application/messages/api.h"
#include <iostream>

namespace claw::prototyping {

ClientTestSystem::ClientTestSystem(const core::SystemContext& ctx):
System(ctx),
chat_input_system_{*ctx.Get<chat::client::ChatInputSystem>()},
tcp_system_{*ctx.Get<chat::server::ChatClientTcpSystem>()} {
  tcp_system_.RegisterMessageHandler<chat::api::PrintMessage>([&](communication::ConnectionID id, const chat::api::PrintMessage& message) {
    if (!ping_active_) return;
    std::cout << "[" << id << "]" << "server says: " << message.value << std::endl;
  });

  tcp_system_.RegisterMessageHandler<chat::api::GetChatsResponseMessage>([&](communication::ConnectionID, const chat::api::GetChatsResponseMessage& message) {
    std::cout << "received chats: " << std::endl;
    for (const auto& chat_message : message.messages) {
      std::cout << chat_message.user_id << ": " << chat_message.content << std::endl;
    }
  });

  tcp_system_.RegisterMessageHandler<chat::api::AuthenticateUserResponseMessage>([&](communication::ConnectionID, const chat::api::AuthenticateUserResponseMessage& message) {
    std::cout << "login successful" << std::endl;
    user_ = std::make_unique<chat::api::User>(std::move(message.user));
  });
}

void ClientTestSystem::Initialize() {

}

void ClientTestSystem::Update() {
  static std::vector<std::string> parameter_buffer;
  static std::string line;

  if (!chat_input_system_.GetLine(line)) return;

  if (line.starts_with("connect")) {
    ParseCommand(line, parameter_buffer);
    if (parameter_buffer.size() <= 1) return;

    tcp_system_.Connect({parameter_buffer[1], "1338"}, [this](communication::ConnectionID id) {
     connection_id_ = id;
    });

    return;
  }

  if (connection_id_ <= 0) return;

  if (line.starts_with("write")) {
    const std::string message = line.substr(std::strlen("write"));
    tcp_system_.Send<chat::api::WriteChatMessage>(connection_id_, {1, {1, message}});
    return;
  }

  if (line.starts_with("get")) {
    const std::string channel_id = line.substr(std::strlen("get"));
    const auto channel_id_param = static_cast<chat::api::PersistenceID>(std::stoul(channel_id));
    tcp_system_.Send<chat::api::GetChatsRequestMessage>(connection_id_, {channel_id_param});
    return;
  }

  if (line.starts_with("create")) {
    const std::string name = line.substr(std::strlen("create"));
    tcp_system_.Send<chat::api::CreateChannelMessage>(connection_id_, {name});
    return;
  }

  if (line.starts_with("adduser")) {
    ParseCommand(line, parameter_buffer);
    if (parameter_buffer.size() <= 2) return;

    const auto& name = parameter_buffer[1];
    const auto& password = parameter_buffer[2];
    tcp_system_.Send<chat::api::CreateUserMessage>(connection_id_, {name, password});
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

  if (line.starts_with("ping")) {
    ping_active_ = !ping_active_;
    return;
  }

  tcp_system_.Send<chat::api::PrintMessage>(connection_id_, {line});
}

void ClientTestSystem::ParseCommand(std::string_view view, std::vector<std::string>& result) {
  result.clear();

  for (auto split : view | std::views::split(':')) {
    result.emplace_back(split.begin(), split.end());
  }
}

}