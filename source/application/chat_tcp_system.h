#pragma once

#include <claw/network-modules/connectors/boost_sync/sync_boost_tcp_resolver.h>
#include <claw/network-modules/connectors/boost_async/boost_tcp_acceptor.h>

#include <claw/network/tcp/tcp_system.h>
#include <claw/network/tcp/tcp_system_builder.h>
#include <claw/network-modules/codecs/json_network_codec.h>

#include "messages/json_macros.h"

namespace claw::network::modules {
class BoostTcpAcceptor;
}
namespace claw::chat::server {

using MessageTypes = std::tuple<
    api::PrintMessage,
    api::WriteChatMessage,
    api::GetChatsRequestMessage,
    api::GetChatsResponseMessage,
    api::CreateChannelMessage,
    api::CreateUserMessage,
    api::GetUserRequestMessage,
    api::GetUserResponseMessage,
    api::AuthenticateUserRequestMessage,
    api::AuthenticateUserResponseMessage
>;

// using ChatServerTcpSystem = network::tcp::TcpSystemBuilder<
//     network::modules::SyncBoostTcpAcceptor,
//     JsonNetworkCodec,
//     MessageTypes
// >::Type;
//
using ChatClientTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::SyncBoostTcpResolver,
    JsonNetworkCodec,
    MessageTypes
>::Type;

using ChatServerTcpSystem = network::tcp::TcpSystemBuilder<
    network::modules::BoostTcpAcceptor,
    JsonNetworkCodec,
    MessageTypes
>::Type;

// using ChatClientTcpSystem = network::tcp::TcpSystemBuilder<
//     network::modules::BoostTcpAcceptor,
//     JsonNetworkCodec,
//     MessageTypes
// >::Type;
}