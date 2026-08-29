#include "chat_client_application.hpp"

#include "../commands/client_commands_system.hpp"
#include "../model/model_system.hpp"
#include "../session/session_system.hpp"
#include "../ui/ui_system.hpp"
#include "client_rpc_system.hpp"
#include "client_tcp_system.hpp"

#include <ebroschin/logging-modules/stacktrace/boost_stacktrace.hpp>
#include <ebroschin/logging-modules/spdlog/spdlog-logger.hpp>

namespace ebroschin::chatcore::client {

void ChatClientApplication::PrepareContext() {
  logging::Log::SetLogger<logging::modules::SpdlogLogger>();
  logging::Log::Info("Starting initialization");

  auto* scheduling_system = ctx_.Register<scheduling::SchedulingSystem>();
  ctx_.Register<ClientTcpSystem>(executor_);

  ctx_.Register<ClientRpcSystem>(*scheduling_system, executor_);
  ctx_.Register<ModelSystem>();
  ctx_.Register<SessionSystem>(*this);
  ctx_.Register<ClientCommandsSystem>();
  ctx_.Register<UiSystem>();
}

void ChatClientApplication::OnContextInitialized() {
  application_thread_ = std::jthread{[this]
  (const std::stop_token& st)
  {
    while (!st.stop_requested()) {
      executor_.ProcessBlocking();
    }
  }};

  auto& tcp_system = ctx_.Require<ClientTcpSystem>();
  auto subscription = tcp_system.Subscribe<network::tcp::ConnectionCreated>([this](const auto& event) {
    if (!event.connection_id) {
      logging::Log::Info() << "Connection to chat server failed.";
      return;
    }

    connection_id_.emplace(*event.connection_id);
    logging::Log::Info() << "Successfully connected to chat server.";
  });
  subscriptions_.emplace_back(std::move(subscription));

  subscription = tcp_system.Subscribe<network::tcp::ConnectionFailed>([](const auto& event) {
    logging::Log::Error() << "Connection to chat server failed: " << event.data.error;
  });
  subscriptions_.emplace_back(std::move(subscription));

  subscription = tcp_system.Subscribe<network::tcp::ConnectionRemoved>([this](const auto&) {
    connection_id_.reset();
    ctx_.Require<SessionSystem>().ResetUser();
    logging::Log::Info() << "Lost connection to chat server.";
    ctx_.Require<ModelSystem>().SetChannelName(std::nullopt);
  });
  subscriptions_.emplace_back(std::move(subscription));
}

void ChatClientApplication::OnContextDeinitialized() {
  executor_.Stop();
  application_thread_ = {};
}

void ChatClientApplication::HandleTerminate() {
  logging::Log::Shutdown();

  logging::Log::SetLogger<logging::modules::SpdlogLogger>();
  logging::modules::BoostStacktrace::PrintExceptionStacktrace();
  logging::Log::Shutdown();
}

}