#pragma once

#include <string>
#include <span>
#include <vector>

namespace claw::communication {

class TcpConnection {
public:
  virtual ~TcpConnection() = default;

  virtual bool HasData() = 0;
  virtual bool IsOpen() = 0;
  virtual void SendMessage(std::span<const std::byte> bytes) = 0;
  virtual std::vector<std::byte> ReadMessage() = 0;
};

}