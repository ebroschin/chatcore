#pragma once

#include "../boost_communication/sync/boost_tcp_client.h"
#include "../boost_communication/sync/boost_tcp_server.h"
#include <claw/network/tcp/tcp_system.h>
#include <claw/network/tcp/tcp_system_builder.h>

#include "../codec/test_codec.h"
#include "messages/json_macros.h"

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

using ChatServerTcpSystem = network::tcp::TcpSystemBuilder<
    communication::BoostTcpServer,
    TestCodec,
    MessageTypes
>::Type;

using ChatClientTcpSystem = network::tcp::TcpSystemBuilder<
    communication::BoostTcpClient,
    TestCodec,
    MessageTypes
>::Type;
}