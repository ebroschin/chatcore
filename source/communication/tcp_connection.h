#pragma once

#include <span>
#include <functional>

namespace claw::communication {

class TcpConnection {
public:
  using ReceiverCallback = std::function<void(std::span<const std::byte>)>;

  virtual ~TcpConnection() = default;

  virtual bool IsOpen() = 0;
  virtual void Start(ReceiverCallback callback) = 0;
  virtual void SendMessage(std::span<const std::byte> bytes) = 0;

protected:
  ReceiverCallback callback_ = nullptr;
};

}