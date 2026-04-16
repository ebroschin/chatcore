#include "application_system.h"

#include <ebroschin/core/system_context.h>

#include "../users/user_server_system.h"
#include "chat_server_application.h"
#include "chat_tcp_system.h"
#include <ebroschin/logging/log.hpp>

namespace ebroschin::chatcore::server {

ApplicationSystem::ApplicationSystem(const core::SystemContext& ctx, ChatServerApplication& app):
  System{ctx},
  app_{app},
  tcp_system_{ctx.Require<ChatServerTcpSystem>()},
  message_handler_{tcp_system_.GetMessageProcessor().GetMessageHandler()}
{}

void ApplicationSystem::Initialize() {
  application_thread_ = std::jthread{[this](const std::stop_token& st) {
    auto& processor = tcp_system_.GetMessageProcessor();
    while (!st.stop_requested()) {
      processor.ProcessBlocking();
    }
  }};

  connection_event_handler_ = std::make_unique<ConnectionEventHandler>(ctx_.Require<UserServerSystem>());
  const auto& arguments = app_.GetArguments();
  tcp_system_.Connect({arguments.GetIp(), arguments.GetPort()}, connection_event_handler_.get());

  logging::Log::Info("ChatCore server started, accepting clients");
}

void ApplicationSystem::Deinitialize() {
  auto& processor = tcp_system_.GetMessageProcessor();
  processor.Stop();

  application_thread_ = {};
}

void ApplicationSystem::Shutdown() const noexcept {
  app_.Quit();
}

void ApplicationSystem::HandleRpcError(network::ConnectionId connection_id, network::RequestId request_id, const std::string& message) const {
  logging::Log::Debug("RPC Error: " + message);
  tcp_system_.Send<api::ErrorResponseMessage>(connection_id, {request_id, message});
}

}