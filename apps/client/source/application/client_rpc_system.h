#pragma once

#include "client_tcp_system.h"

#include <ebroschin/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.h>
#include <ebroschin/network/rpc/rpc_system.h>

namespace ebroschin::chatcore::client {

using ClientRpcSystem = network::rpc::RpcSystem<ClientTcpSystem, network::modules::SchedulerRpcTimeoutHandler>;

}