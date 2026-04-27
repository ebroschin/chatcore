#include "test_client.hpp"

#include "../application/application_system.hpp"

using namespace std::chrono_literals;

namespace ebroschin::chatcore::tester {

TestClient::TestClient(ApplicationSystem& app_system,
  ClientTcpSystem& tcp_system,
  ClientRpcSystem& rpc_system,
  scheduling::SchedulingSystem& scheduling_system,
  RootClient& root_client,
  const std::string& name,
  api::PersistenceId channel_id):
  Client(app_system, tcp_system, rpc_system, name),
  scheduling_system_(scheduling_system),
  root_client_(root_client),
  channel_id_(channel_id)
{}

void TestClient::OnPrepared() {
  auto& message_handler = app_system_.GetMessageHandler();

  receive_chat_signal_handle_ = message_handler.Subscribe<api::ReceiveChatMessage>(
  [this](network::ConnectionId connection_id, const api::ReceiveChatMessage& message) {
    if (connection_id_ != connection_id) return;
    if (message.channel_id != channel_id_) return;
    if (message.user_id != user_->id) return;

    const auto message_id = std::stoull(message.content);
    incoming_messages_.try_emplace(message_id, TrackedMessage{ steady_clock::now(), message_id });
  });

  auto join_channel_call = rpc_system_.Prepare<api::JoinChatChannelRequestMessage>(*connection_id_, channel_id_);
  join_channel_call.OnSuccess([this](const api::JoinChatChannelResponseMessage&) {
    root_client_.SetClientReady(*connection_id_);
  });

  RegisterDefaultErrorHandler(join_channel_call);
  RegisterDefaultTimeoutHandler(join_channel_call, "Join channel request timed out");
  join_channel_call.Call();
}

void TestClient::Start(steady_clock::duration phase) {
  phase_ = phase;
  current_send_task_id_ = scheduling_system_.ScheduleAfter(phase, [this] { Send(); });
}

void TestClient::Stop() const {
  scheduling_system_.RemoveTask(current_send_task_id_);
  tcp_system_.Disconnect(*connection_id_);
}

ClientReport TestClient::Evaluate() {
  std::vector<steady_clock::duration> roundtrip_times;

  for(const auto& pair : outgoing_messages_) {
    auto it = incoming_messages_.find(pair.first);
    if (it == incoming_messages_.end()) continue;

    auto roundtrip_time = it->second.time_point - pair.second.time_point;
    roundtrip_times.emplace_back(roundtrip_time);
  }

  return ClientReport{std::move(roundtrip_times), outgoing_messages_.size()};
}

void TestClient::Send() {
  static std::uint64_t next_id{0};
  const auto message_id = next_id++;
  tcp_system_.Send(*connection_id_, api::WriteChatMessage{ std::to_string(message_id) });
  outgoing_messages_.try_emplace(message_id, TrackedMessage{ steady_clock::now(), message_id });
  current_send_task_id_ = scheduling_system_.ScheduleAfter(phase_, [this] { Send(); });
}

}