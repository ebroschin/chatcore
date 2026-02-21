#pragma once

#include <claw/core/system.h>
#include <thread>
#include <functional>
#include "commons.h"

namespace claw::chat::server {

class ApplicationSystem final : public core::System {
public:
  explicit ApplicationSystem(const core::SystemContext& ctx);

  void Initialize() override;
  void Deinitialize() override;

  template<typename TMessage>
  void RegisterMessageHandler(std::function<void(network::ConnectionId id, const TMessage&)> function) {
    assert(processor_);
    processor_->RegisterMessageHandler<TMessage>(function);
  }

private:
  ChatServerTcpSystem::MessageProcessor* processor_{nullptr};
  std::jthread application_thread_{};
};

}