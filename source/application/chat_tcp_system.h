#pragma once

#include "../communication/tcp_system.h"
#include "../boost_communication/boost_tcp_server.h"
#include "../boost_communication/boost_tcp_client.h"

#include "../communication/tcp_system_builder.h"
#include "../codec/test_codec.h"

namespace claw::chat::server {

using MessageTypes = std::tuple<
    communication::TestMessage,
    communication::AnotherMessage,
    communication::OtherMessage
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