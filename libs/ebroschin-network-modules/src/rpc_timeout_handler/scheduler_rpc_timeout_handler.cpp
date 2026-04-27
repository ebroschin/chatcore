#include <ebroschin/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.hpp>

namespace ebroschin::network::modules {

SchedulerRpcTimeoutHandler::SchedulerRpcTimeoutHandler(scheduling::SchedulingSystem& scheduling_system) noexcept:
    scheduling_system_(scheduling_system)
{}

void SchedulerRpcTimeoutHandler::ScheduleTimeout(RequestId request_id, steady_clock::duration duration, std::function<void()> callback) {
  if (!callback) return;

  const auto handle = scheduling_system_.ScheduleAfter(duration, [callback = std::move(callback)] {
    callback();
  });

  timeouts_.emplace(request_id, handle);
}

void SchedulerRpcTimeoutHandler::CancelTimeout(RequestId request_id) {
  const auto it = timeouts_.find(request_id);
  if (it == timeouts_.end()) return;

  scheduling_system_.RemoveTask(it->second);
  timeouts_.erase(it);
}

}