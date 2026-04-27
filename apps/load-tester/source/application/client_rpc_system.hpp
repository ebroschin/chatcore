#pragma once

#include <ebroschin/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.hpp>
#include <ebroschin/network/rpc/rpc_system.hpp>

#include "client_tcp_system.hpp"

namespace ebroschin::chatcore::tester {

using ClientRpcSystem = network::rpc::RpcSystem<ClientTcpSystem, network::modules::SchedulerRpcTimeoutHandler>;

}