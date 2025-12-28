#pragma once

#include "../boost_communication/boost_tcp_connection.h"

#include "boost/asio.hpp"
#include <claw/core/application.h>
#include <claw/core/system.h>

using boost::asio::ip::tcp;

namespace claw::communication {

class ClientCommunicationSystem final : public core::System {
public:
  explicit ClientCommunicationSystem(const core::SystemContext& ctx, core::Application& app)
    : System(ctx), app_{app}
  {}

  void Initialize() override;
  void Update() override;

  void SendMessage(const std::string& message);
  
private:
  core::Application& app_;
  boost::asio::io_context io_context_{};
  std::unique_ptr<chat::server::BoostTcpConnection> connection_;
};

}