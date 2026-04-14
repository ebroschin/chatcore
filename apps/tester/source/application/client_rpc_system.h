#pragma once

#include <claw/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.h>
#include <claw/network/rpc/rpc_system.h>

#include "client_tcp_system.h"

namespace claw::chat::tester {

using ClientRpcSystem = network::rpc::RpcSystem<ClientTcpSystem, network::modules::SchedulerRpcTimeoutHandler>;

}