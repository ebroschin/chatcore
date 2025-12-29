#include "client_test_system.h"

#include "../communication/tcp_system.h"
#include <claw/core/system_context.h>

namespace claw::prototyping {

ClientTestSystem::ClientTestSystem(const core::SystemContext& ctx)
  : System(ctx)
{
  ctx.Get<communication::TcpSystem>()->RegisterMessageHandler<PingMessageHandler>("ping");
}

}