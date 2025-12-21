#pragma once

#include "boost/asio.hpp"
#include <claw/core/system.h>
#include <claw/core/application.h>

using boost::asio::ip::tcp;

namespace claw::communication {

class CommunicationSystem final : public core::System {
public:
  explicit CommunicationSystem(const core::SystemContext& ctx, core::Application& app)
  : System(ctx), app_{app} {}

  void Initialize() override;
  void Update() override;
  void Quit() override;
private:
  core::Application& app_;
  boost::asio::io_context io_context_{};
  tcp::acceptor acceptor_{io_context_, tcp::endpoint(tcp::v4(), 1338)};
  tcp::socket socket_{io_context_};
};

}