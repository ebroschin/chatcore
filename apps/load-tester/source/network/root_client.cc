#include "root_client.h"

#include <chrono>

#include "../application/application_system.h"
#include "test_client.h"

using namespace std::chrono_literals;

namespace ebroschin::chatcore::tester {

RootClient::RootClient(ApplicationSystem& app_system,
  ClientTcpSystem& tcp_system,
  ClientRpcSystem& rpc_system,
  scheduling::SchedulingSystem& scheduling_system,
  const std::string& name):
  Client(app_system, tcp_system, rpc_system, name),
  scheduling_system_(scheduling_system)
{}

void RootClient::OnPrepared() {
  auto create_channel_call = rpc_system_.Prepare<api::CreateChannelRequestMessage>(*connection_id_, "test-channel");
  create_channel_call.OnSuccess([this](const api::CreateChannelResponseMessage& message) {
    test_channel_id_ = message.channel.id;
    HandleRootClientReady();
  });

  RegisterDefaultErrorHandler(create_channel_call);
  RegisterDefaultTimeoutHandler(create_channel_call, "Create channel request timed out");
  create_channel_call.Call();
}

void RootClient::HandleRootClientReady() {
  const auto client_count = app_system_.GetArguments().GetClientCount();
  for (unsigned int i = 0; i < client_count; i++) {
    clients_.emplace_back(std::make_unique<TestClient>(app_system_,
      tcp_system_,
      rpc_system_,
      scheduling_system_,
      *this,
      "tester" + std::to_string(i),
      test_channel_id_));
  }

  for (const auto& client : clients_) {
    client->Prepare();
  }
}

void RootClient::Evaluate() {
  std::ranges::for_each(clients_, &TestClient::Stop);

  std::vector<ClientReport> reports;
  reports.reserve(clients_.size());
  for (const auto& client : clients_) {
    reports.emplace_back(client->Evaluate());
  }

  std::vector<steady_clock::duration> latencies;
  latencies.reserve(std::accumulate(reports.begin(), reports.end(), std::size_t{0},
    [](std::size_t result, const ClientReport& report)
  {
    return result + report.roundtrip_times.size();
  }));

  std::size_t total_sent{0};
  std::size_t total_failed{0};
  for (const auto& report : reports) {
    total_sent += report.sent_count;
    total_failed += report.FailedCount();
    std::ranges::copy(report.roundtrip_times, std::back_inserter(latencies));
  }

  ebroschin::logging::Log::Info() << "total sent: " << total_sent;
  ebroschin::logging::Log::Info() << "total completed: " << latencies.size();
  ebroschin::logging::Log::Info() << "total failed: " << total_failed;

  const auto to_us = [](steady_clock::duration d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
  };

  ebroschin::logging::Log::Info() << "p50: " << to_us(CalculatePercentile(latencies, 0.5)) << "us";
  ebroschin::logging::Log::Info() << "p95: " << to_us(CalculatePercentile(latencies, 0.95)) << "us";
  ebroschin::logging::Log::Info() << "p99: " << to_us(CalculatePercentile(latencies, 0.99)) << "us";
  ebroschin::logging::Log::Info() << "max: " << to_us(CalculatePercentile(latencies, 1.0)) << "us";
}

steady_clock::duration RootClient::CalculatePercentile(std::vector<steady_clock::duration> latencies, double normalized_percentage) {
  if (latencies.empty()) return steady_clock::duration::zero();

  const auto fractional_index = std::floor(normalized_percentage * static_cast<double>(latencies.size() - 1));
  const auto index = static_cast<std::size_t>(fractional_index);
  const auto n = latencies.begin() + static_cast<long>(fractional_index);
  std::ranges::nth_element(latencies, n);

  return latencies[index];
}

void RootClient::SetClientReady(network::ConnectionId id) {
  connected_clients_.emplace(id);

  if (connected_clients_.size() != clients_.size()) return;
  ebroschin::logging::Log::Info() << "Clients prepared, running load test";

  auto phase = 1000ms;
  const auto stagger_duration = phase / clients_.size();
  for (std::size_t i = 0; i < clients_.size(); i++) {
    scheduling_system_.ScheduleAfter(stagger_duration * i, [this, i, phase] {
      clients_[i]->Start(phase);
    });
  }

  scheduling_system_.ScheduleAfter(10s, [this] { Evaluate(); });
}

}