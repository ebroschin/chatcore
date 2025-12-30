#include "client_test_system.h"

#include "../communication/tcp_system.h"
#include "../application/chat_tcp_system.h"
#include <claw/core/system_context.h>
#include "chat_input_system.h"

namespace claw::prototyping {

ClientTestSystem::ClientTestSystem(const core::SystemContext& ctx):
System(ctx),
chat_input_system_{*ctx.Get<chat::client::ChatInputSystem>()},
tcp_system_{*ctx.Get<chat::server::ChatClientTcpSystem>()} {
  tcp_system_.RegisterMessageHandler<PingMessageHandler>("ping");
}

void ClientTestSystem::Update() {
  static std::string line;
  if (!chat_input_system_.GetLine(line)) return;
  tcp_system_.SendMessage(line);
}


}