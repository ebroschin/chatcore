#pragma once

#include "server.h"

namespace claw::communication {

class TCPServer : public Server {
public:
  virtual void Initialize() = 0;
  virtual void Update() = 0;
  virtual void Deinitialize() = 0;
};

}
