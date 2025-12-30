#pragma once

#include "../communication/tcp_system.h"
#include "../boost_communication/boost_tcp_server.h"

namespace claw::chat::server {
using ChatServerTcpSystem = communication::TcpSystem<communication::BoostTcpServer>;
}