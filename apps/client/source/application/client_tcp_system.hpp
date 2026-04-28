#pragma once

#include <ebroschin/network-modules/codecs/json_network_codec.hpp>
#include <ebroschin/network-modules/connectors/boost_async/boost_tcp_resolver.hpp>
#include <ebroschin/network-modules/message_handler/observable_message_handler.hpp>
#include <ebroschin/network/tcp/tcp_system_builder.hpp>
#include <ebroschin/chat/json_api.hpp>

namespace ebroschin::chatcore::client {

using ClientTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::BoostTcpResolver,
    network::modules::JsonNetworkCodec,
    network::modules::ObservableMessageHandler,
    api::MessageTypes
>::Type;

}