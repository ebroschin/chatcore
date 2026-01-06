#include "client_test_system.h"

#include "../communication/tcp_system.h"
#include "../application/chat_tcp_system.h"
#include <claw/core/system_context.h>
#include "chat_input_system.h"
#include "../application/messages/api.h"

namespace claw::prototyping {

ClientTestSystem::ClientTestSystem(const core::SystemContext& ctx):
System(ctx),
chat_input_system_{*ctx.Get<chat::client::ChatInputSystem>()},
tcp_system_{*ctx.Get<chat::server::ChatClientTcpSystem>()} {
  // tcp_system_.RegisterMessageHandler<communication::TestMessage>([&](const communication::TestMessage& message) {
  //   std::cout << "ping received: " << message.value << std::endl;
  // });

  tcp_system_.RegisterMessageHandler<chat::api::GetChatsResponseMessage>([&](const chat::api::GetChatsResponseMessage& message) {
    std::cout << "received chats: " << std::endl;
    for (const auto& chat_message : message.messages) {
      std::cout << chat_message.user_id << ": " << chat_message.content << std::endl;
    }
  });
}

void ClientTestSystem::Update() {
  static std::string line;
  if (!chat_input_system_.GetLine(line)) return;

  if (line.starts_with("write")) {
    const std::string message = line.substr(std::strlen("write"));
    tcp_system_.SendMessage<chat::api::WriteChatMessage>({1, {1, message}});
    return;
  }

  if (line.starts_with("get")) {
    const std::string channel_id = line.substr(std::strlen("get"));
    const auto channel_id_param = static_cast<std::uint32_t>(std::stoul(channel_id));
    tcp_system_.SendMessage<chat::api::GetChatsRequestMessage>({channel_id_param});
    return;
  }

  if (line.starts_with("create")) {
    const std::string name = line.substr(std::strlen("create"));
    tcp_system_.SendMessage<chat::api::CreateChannelMessage>({name});
    return;
  }

  tcp_system_.SendMessage<communication::TestMessage>({line});
}

}