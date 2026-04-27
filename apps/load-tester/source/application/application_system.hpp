#pragma once

#include "../testing/root_client.hpp"
#include "client_tcp_system.hpp"
#include "load_tester_application.hpp"

#include <ebroschin/core/system.hpp>

namespace ebroschin::chatcore::tester {

class ApplicationSystem final : public core::System {
public:
  explicit ApplicationSystem(const core::SystemContext& ctx, LoadTesterApplication& app);

  void Initialize() override;
  void Deinitialize() override;

  [[nodiscard]] ClientTcpSystem::MessageHandler& GetMessageHandler() const noexcept
  { return message_handler_; }

  [[nodiscard]] const LoadTesterArguments& GetArguments() const noexcept
  { return app_.GetArguments(); }

  void Quit() const;

private:
  LoadTesterApplication& app_;
  ClientTcpSystem& tcp_system_;
  ClientRpcSystem& rpc_system_;
  ClientTcpSystem::MessageHandler& message_handler_;

  std::jthread application_thread_{};
  std::unique_ptr<RootClient> root_client_{};
};

}