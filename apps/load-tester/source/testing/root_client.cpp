#include "root_client.hpp"

#include "test_client.hpp"
#include "../application/load_tester_application.hpp"

#include <chrono>

using namespace std::chrono_literals;

namespace ebroschin::chatcore::tester {

RootClient::RootClient(LoadTesterApplication& app,
  core::SystemContext& ctx,
  const std::string& name,
  core::QueuedExecutor& executor) noexcept:
  Client{app, ctx, name, executor},
  scheduling_system_{ctx.Require<scheduling::SchedulingSystem>()}
{}

void RootClient::OnPrepared() {
  auto create_channel_call = rpc_system_.Prepare<api::CreateChannelRequestMessage>(*connection_id_, "test-channel");
  create_channel_call.OnSuccess([this]
  (const api::CreateChannelResponseMessage& message)
  {
    test_channel_id_ = message.channel.id;
    HandleRootClientReady();
  });

  RegisterDefaultErrorHandler(create_channel_call);
  RegisterDefaultTimeoutHandler(create_channel_call, "Create channel request timed out");
  create_channel_call.Call();
}

void RootClient::HandleRootClientReady() {
  const auto client_count = app_.GetArguments().GetClientCount();
  for (unsigned int i = 0; i < client_count; i++) {
    clients_.emplace_back(std::make_unique<TestClient>(app_,
      ctx_,
      executor_,
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

  std::vector<ClientReport> reports{};
  reports.reserve(clients_.size());

  for (const auto& client : clients_) {
    reports.emplace_back(client->Evaluate());
  }

  std::vector<steady_clock::duration> latencies{};
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

  logging::Log::Info() << "total sent: " << total_sent;
  logging::Log::Info() << "total completed: " << latencies.size();
  logging::Log::Info() << "total failed: " << total_failed;

  const auto to_us = [](steady_clock::duration d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
  };

  logging::Log::Info() << "p50: " << to_us(ClientReport::CalculatePercentile(latencies, 0.5)) << "us";
  logging::Log::Info() << "p95: " << to_us(ClientReport::CalculatePercentile(latencies, 0.95)) << "us";
  logging::Log::Info() << "p99: " << to_us(ClientReport::CalculatePercentile(latencies, 0.99)) << "us";
  logging::Log::Info() << "max: " << to_us(ClientReport::CalculatePercentile(latencies, 1.0)) << "us";

  app_.Quit();
}

void RootClient::SetClientReady(network::ConnectionId id) {
  connected_clients_.emplace(id);

  if (connected_clients_.size() != clients_.size()) return;
  logging::Log::Info() << "Clients prepared, running load test";

  constexpr auto phase = 1000ms;
  const auto stagger_duration = phase / clients_.size();
  for (std::size_t i = 0; i < clients_.size(); i++) {
    scheduling_system_.ScheduleAfter(stagger_duration * i, [this, i, phase] {
      clients_[i]->Start(phase);
    }, &executor_);
  }

  scheduling_system_.ScheduleAfter(10s, [this] { Evaluate(); }, &executor_);
}

}