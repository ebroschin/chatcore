#pragma once

#include "../communication/tcp_system.h"
#include "../boost_communication/boost_tcp_server.h"
#include "../boost_communication/boost_tcp_client.h"

namespace claw::chat::server {
using ChatServerTcpSystem = communication::TcpSystem<communication::BoostTcpServer>;
using ChatClientTcpSystem = communication::TcpSystem<communication::BoostTcpClient>;
}