#include "chat_server_application.h"

#include "../boost_communication/boost_tcp_server.h"
#include "../communication/tcp_server_system.h"
#include "../prototyping/client_communication_system.h"
#include "../prototyping/prototyping_system.h"

#include <iostream>

//#define SERVER_SIDE

namespace claw::chat::server {

void ChatServerApplication::Initialize() {
  ctx_->Register<prototyping::PrototypingSystem>(argument_);

#ifdef SERVER_SIDE
  ctx_->Register<communication::TCPServerSystem<communication::BoostTCPServer>>(*this, "0.0.0.0", 1338);
#else
  ctx_->Register<communication::ClientCommunicationSystem>(*this);
#endif
}

void ChatServerApplication::HandleTerminate() {
  std::cout << "woops" << std::endl; //TODO reuse code from client (move to library)
}

}