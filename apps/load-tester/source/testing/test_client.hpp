#pragma once

#include <ebroschin/scheduling/scheduling_system.hpp>

#include <string>
#include <unordered_map>

#include "../application/client_tcp_system.hpp"
#include "client.hpp"
#include "client_report.hpp"
#include "tracked_message.hpp"

namespace ebroschin::chatcore::tester {

class ApplicationSystem;
class RootClient;

class TestClient final : public Client {
public:
  explicit TestClient(ApplicationSystem& app_system,
    ClientTcpSystem& tcp_system,
    ClientRpcSystem& rpc_system,
    scheduling::SchedulingSystem& scheduling_system,
    RootClient& root_client,
    const std::string& name,
    api::PersistenceId channel_id);

  void Start(steady_clock::duration phase);
  void Stop() const;
  ClientReport Evaluate();

protected:
  void OnPrepared() override;

private:
  void Send();

  scheduling::SchedulingSystem& scheduling_system_;
  RootClient& root_client_;
  api::PersistenceId channel_id_;

  utility::SignalSubscription receive_chat_signal_handle_{};
  scheduling::TaskId current_send_task_id_{};
  steady_clock::duration phase_{};

  std::unordered_map<std::uint64_t, TrackedMessage> incoming_messages_{};
  std::unordered_map<std::uint64_t, TrackedMessage> outgoing_messages_{};
};

}