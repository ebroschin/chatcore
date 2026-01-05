#pragma once

#include "../communication/tcp_system.h"
#include "../boost_communication/boost_tcp_server.h"
#include "../boost_communication/boost_tcp_client.h"

#include "../communication/tcp_system_parameters.h"

namespace claw::chat::server {

using MessageTypes = std::tuple<
    communication::TestMessage,
    communication::AnotherMessage,
    communication::OtherMessage
>;

using ChatServerTcpSystem = communication::TcpSystemParameters<
    communication::BoostTcpServer,
    communication::TestSerializer,
    MessageTypes
>::Type;

using ChatClientTcpSystem = communication::TcpSystemParameters<
    communication::BoostTcpClient,
    communication::TestSerializer,
    MessageTypes
>::Type;
}