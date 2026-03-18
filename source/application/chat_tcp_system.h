#pragma once

#include <claw/network-modules/connectors/boost_async/boost_tcp_resolver.h>
#include <claw/network-modules/connectors/boost_async/boost_tcp_acceptor.h>
#include <claw/network/tcp/tcp_system.h>
#include <claw/network/tcp/tcp_system_builder.h>
#include <claw/network-modules/codecs/json_network_codec.h>
#include <claw/network-modules/message_handler/direct_message_handler.h>
#include <claw/chat/json_api.h>

namespace claw::chat::server {

using ChatServerTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::BoostTcpAcceptor,
    network::modules::JsonNetworkCodec,
    network::modules::DirectMessageHandler,
    api::MessageTypes
>::Type;

}