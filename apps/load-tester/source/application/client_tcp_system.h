#pragma once

#include <claw/network-modules/codecs/json_network_codec.h>
#include <claw/network/tcp/tcp_system.h>
#include <claw/network/tcp/tcp_system_builder.h>
#include <claw/network-modules/connectors/boost_async/boost_tcp_resolver.h>
#include <claw/network-modules/message_handler/observable_message_handler.h>
#include <claw/chat/json_api.h>

namespace claw::chat::tester {

using ClientTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::BoostTcpResolver,
    network::modules::JsonNetworkCodec,
    network::modules::ObservableMessageHandler,
    api::MessageTypes
>::Type;

}