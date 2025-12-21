#include "communication_system.h"

#include "../prototyping/prototyping_system.h"

#include <iostream>

namespace claw::communication {

void CommunicationSystem::Initialize() {
  std::cout << "started server" << std::endl;
}

void CommunicationSystem::Update() {
  std::cout << "waiting for clients" << std::endl;
  tcp::socket socket(io_context_);
  acceptor_.accept(socket);
  std::cout << "client connected!" << std::endl;

  std::string message = ctx_.Get<prototyping::PrototypingSystem>()->Argument();
  boost::asio::write(socket, boost::asio::buffer(message));
}

void CommunicationSystem::Quit() {
  std::cout << "server quit" << std::endl;
}

}