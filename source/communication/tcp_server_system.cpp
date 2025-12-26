#include "tcp_server_system.h"

namespace claw::communication {

void TCPServerSystemBase::Initialize() {
  server_->Initialize();
}

void TCPServerSystemBase::Update() {
  server_->Update();
}

void TCPServerSystemBase::Deinitialize() {
  server_->Deinitialize();
}

}