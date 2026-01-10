#pragma once

#include "../communication/tcp_system.h"
#include "../boost_communication/boost_tcp_server.h"
#include "../boost_communication/boost_tcp_client.h"
#include "../communication/tcp_system_builder.h"
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

using ChatServerTcpSystem = communication::TcpSystemBuilder<
    communication::BoostTcpServer,
    TestCodec,
    MessageTypes
>::Type;

using ChatClientTcpSystem = communication::TcpSystemBuilder<
    communication::BoostTcpClient,
    TestCodec,
    MessageTypes
>::Type;
}