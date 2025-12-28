#include "client_test_system.h"

#include "../communication/tcp_system.h"

namespace claw::prototyping {

ClientTestSystem::ClientTestSystem(const core::SystemContext& ctx)
  : System(ctx)
{
  ctx.Get<communication::TcpSystem>()->RegisterMessageHandler<PingMessageHandler>("ping");
}

}