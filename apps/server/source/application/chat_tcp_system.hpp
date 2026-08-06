#pragma once

#include <ebroschin/network-modules/codecs/json_network_codec.hpp>
#include <ebroschin/network-modules/connectors/boost_async/boost_tcp_acceptor.hpp>
#include <ebroschin/network-modules/connectors/boost_async/boost_tcp_resolver.hpp>
#include <ebroschin/network/tcp/tcp_system.hpp>
#include <ebroschin/network/tcp/tcp_system_builder.hpp>
#include <ebroschin/chat/json_api.hpp>

namespace ebroschin::chatcore::server {

using ChatServerTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::BoostTcpAcceptor,
    network::modules::JsonNetworkCodec,
    api::MessageTypes
>::Type;

}