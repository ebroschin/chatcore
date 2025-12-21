#pragma once

#include <claw/core/system.h>
#include <claw/core/application.h>
#include "boost/asio.hpp"

using boost::asio::ip::tcp;

namespace claw::communication {

class ClientCommunicationSystem final : public core::System {
public:
  explicit ClientCommunicationSystem(const core::SystemContext& ctx, core::Application& app)
    : System(ctx), app_{app}
  {}

  void Initialize() override;
  void Update() override;
  void Quit() override;
private:
  core::Application& app_;
  boost::asio::io_context io_context_{};
  tcp::socket socket_{io_context_};
};

}