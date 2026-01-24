#pragma once

#include <claw/network-modules/connectors/boost_sync/sync_boost_tcp_resolver.h>
#include <claw/network-modules/connectors/boost_async/boost_tcp_acceptor.h>
#include <claw/network/tcp/tcp_system.h>
#include <claw/network/tcp/tcp_system_builder.h>
#include <claw/network-modules/codecs/json_network_codec.h>
#include <claw/chat/json_api.h>

namespace claw::network::modules {
class BoostTcpAcceptor;
}

namespace claw::chat::server {

using ChatClientTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::SyncBoostTcpResolver,
    JsonNetworkCodec,
    api::MessageTypes
>::Type;

using ChatServerTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::BoostTcpAcceptor,
    JsonNetworkCodec,
    api::MessageTypes
>::Type;

}