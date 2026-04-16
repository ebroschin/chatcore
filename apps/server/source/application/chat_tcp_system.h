#pragma once

#include <ebroschin/network-modules/codecs/json_network_codec.h>
#include <ebroschin/network-modules/connectors/boost_async/boost_tcp_acceptor.h>
#include <ebroschin/network-modules/connectors/boost_async/boost_tcp_resolver.h>
#include <ebroschin/network-modules/message_handler/direct_message_handler.h>
#include <ebroschin/network/tcp/tcp_system.h>
#include <ebroschin/network/tcp/tcp_system_builder.h>
#include <ebroschin/chat/json_api.h>

namespace ebroschin::chatcore::server {

using ChatServerTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::BoostTcpAcceptor,
    network::modules::JsonNetworkCodec,
    network::modules::DirectMessageHandler,
    api::MessageTypes
>::Type;

}