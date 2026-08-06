#pragma once

#include "client_tcp_system.hpp"
#include "load_tester_arguments.hpp"
#include "../testing/root_client.hpp"

#include <ebroschin/core/application.hpp>
#include <ebroschin/core/synchronization/queued_executor.hpp>

namespace ebroschin::chatcore::tester {

class LoadTesterApplication final : public core::Application {
public:
  explicit LoadTesterApplication(LoadTesterArguments arguments) noexcept;

  [[nodiscard]] const LoadTesterArguments& GetArguments() const noexcept
  { return arguments_; }

  [[nodiscard]] ClientTcpSystem::EventHandler& GetMessageHandler() const noexcept
  { return ctx_.Require<ClientTcpSystem>().GetMessageHandler(); }

protected:
  void PrepareContext() override;
  void OnContextInitialized() override;
  void OnContextDeinitialized() override;
  void HandleTerminate() override;

private:
  LoadTesterArguments arguments_;

  core::QueuedExecutor executor_{};
  std::jthread application_thread_{};
  std::unique_ptr<RootClient> root_client_{};
};

}