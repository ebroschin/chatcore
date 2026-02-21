#pragma once

#include <claw/network-modules/connectors/boost_async/boost_tcp_resolver.h>
#include <claw/network-modules/connectors/boost_async/boost_tcp_acceptor.h>
#include <claw/network/tcp/tcp_system.h>
#include <claw/network/tcp/tcp_system_builder.h>
#include <claw/network-modules/codecs/json_network_codec.h>
#include <claw/chat/json_api.h>

namespace claw::chat::server {

using ChatClientTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::BoostTcpResolver,
    network::modules::JsonNetworkCodec,
    api::MessageTypes
>::Type;

using ChatServerTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::BoostTcpAcceptor,
    network::modules::JsonNetworkCodec,
    api::MessageTypes
>::Type;

}