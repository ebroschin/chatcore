#pragma once

#include "client.hpp"
#include "test_client.hpp"

#include <unordered_set>
#include <vector>

namespace ebroschin::core {
class QueuedExecutor;
}

namespace ebroschin::chatcore::tester {

class RootClient final : public Client {
public:
  explicit RootClient(LoadTesterApplication& app,
    core::SystemContext& ctx,
    const std::string& name,
    core::QueuedExecutor& executor) noexcept;

  void SetClientReady(network::ConnectionId id);

protected:
  void OnPrepared() override;

private:
  void HandleRootClientReady();
  void Evaluate();

  scheduling::SchedulingSystem& scheduling_system_;

  api::PersistenceId test_channel_id_{};
  std::vector<std::unique_ptr<TestClient>> clients_{};
  std::unordered_set<network::ConnectionId> connected_clients_{};
};

}
