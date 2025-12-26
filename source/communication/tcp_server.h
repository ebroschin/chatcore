#pragma once

namespace claw::communication {

class TCPServer {
public:
  virtual ~TCPServer() = default;

  virtual void Initialize() = 0;
  virtual void Update() = 0;
  virtual void Deinitialize() = 0;
};

}
