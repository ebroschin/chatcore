#pragma once

#include "boost/asio.hpp"
#include <claw/core/system.h>
#include <claw/core/application.h>
#include <concepts>

#include "tcp_server.h"

using boost::asio::ip::tcp;

namespace claw::communication {

class TCPServerSystemBase : public core::System {
public:
  explicit TCPServerSystemBase(const core::SystemContext& ctx,
    core::Application& app,
    std::unique_ptr<TCPServer>&& server)
    : System(ctx), app_{app}, server_{std::move(server)}
  { }

  void Initialize() override;
  void Update() override;
  void Deinitialize() override;

protected:
  core::Application& app_;
  std::unique_ptr<TCPServer> server_;
};

template<typename TServer>
requires std::derived_from<TServer, TCPServer>
class TCPServerSystem final : public TCPServerSystemBase {
public:
  explicit TCPServerSystem(const core::SystemContext& ctx,
    core::Application& app,
    const std::string& address,
    unsigned short port)
    : TCPServerSystemBase(ctx, app, std::make_unique<TServer>(address, port))
  { }
};

}