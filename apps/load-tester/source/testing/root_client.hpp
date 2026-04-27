#pragma once

#include "client.hpp"
#include "test_client.hpp"

#include <ebroschin/scheduling/scheduling_system.hpp>

#include <unordered_set>

namespace ebroschin::chatcore::tester {

class RootClient final : public Client {
public:
  explicit RootClient(ApplicationSystem& app_system,
    ClientTcpSystem& tcp_system,
    ClientRpcSystem& rpc_system,
    scheduling::SchedulingSystem& scheduling_system,
    const std::string& name) noexcept;

  void SetClientReady(network::ConnectionId id);

protected:
  void OnPrepared() override;

private:
  void HandleRootClientReady();
  void Evaluate();
  steady_clock::duration CalculatePercentile(std::vector<steady_clock::duration> latencies, double normalized_percentage);

  scheduling::SchedulingSystem& scheduling_system_;
  api::PersistenceId test_channel_id_{};

  std::vector<std::unique_ptr<TestClient>> clients_{};
  std::unordered_set<network::ConnectionId> connected_clients_{};
};

}
