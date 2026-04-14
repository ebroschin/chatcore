#pragma once

#include <claw/scheduling/scheduling_system.h>

#include <string>
#include <unordered_map>

#include "../application/client_tcp_system.h"
#include "client.h"
#include "client_report.h"
#include "tracked_message.h"

namespace claw::chat::tester {

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