#include "chat_server_application.h"

#include "../communication/communication_system.h"
#include "../prototyping/client_communication_system.h"
#include "../prototyping/prototyping_system.h"

#include <iostream>

//#define SERVER_SIDE

namespace claw::chat::server {

void ChatServerApplication::Initialize() {
  ctx_->Register<prototyping::PrototypingSystem>(argument_);

#ifdef SERVER_SIDE
  ctx_->Register<communication::CommunicationSystem>(*this);
#else
  ctx_->Register<communication::ClientCommunicationSystem>(*this);
#endif
}

void ChatServerApplication::HandleTerminate() {
  std::cout << "woops" << std::endl; //TODO reuse code from client (move to library)
}

}