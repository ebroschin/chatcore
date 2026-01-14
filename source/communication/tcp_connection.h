#pragma once

#include <functional>
#include <memory>
#include <span>

namespace claw::communication {

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
  using ReceiverCallback = std::function<void(std::span<const std::byte>)>;
  using DisconnectCallback = std::function<void()>;

  virtual ~TcpConnection() = default;

  virtual void Start(ReceiverCallback callback, DisconnectCallback disconnect_callback) = 0;
  virtual void SendBytes(std::span<const std::byte> bytes) = 0;

protected:
  ReceiverCallback receiver_callback_ = nullptr;
  DisconnectCallback disconnect_callback_ = nullptr;
};

}