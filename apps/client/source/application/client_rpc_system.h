#pragma once

#include "client_tcp_system.h"

#include <claw/network/rpc/rpc_system.h>
#include <claw/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.h>

namespace claw::chat::client {

using ClientRpcSystem = network::rpc::RpcSystem<ClientTcpSystem, network::modules::SchedulerRpcTimeoutHandler>;

}