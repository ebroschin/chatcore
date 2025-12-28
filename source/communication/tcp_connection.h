#pragma once

#include <string>

namespace claw::communication {

class TcpConnection {
public:
  virtual ~TcpConnection() = default;

  virtual bool HasData() = 0;
  virtual bool IsOpen() = 0;
  virtual void SendMessage(const std::string& message) = 0;
  virtual std::string ReadMessage() = 0;
};

}