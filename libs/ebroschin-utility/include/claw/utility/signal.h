#pragma once

#include <algorithm>
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include "signal_subscription.h"

namespace claw::utility {

class SignalStateBase {
public:
  virtual ~SignalStateBase() = default;
  virtual void Unsubscribe(int id) = 0;
};

template<typename... TArguments>
class SignalState : public SignalStateBase {
public:
  using Slot = std::function<void(TArguments...)>;

  int Subscribe(Slot slot) {
    static int id_counter_ = 0;
    auto id = id_counter_++;
    slots_[id] = std::move(slot);
    return id;
  }

  void Unsubscribe(int id) override {
    slots_.erase(id);
  }

  void Emit(TArguments... arguments) {
    std::vector<int> buffer{};
    buffer.reserve(slots_.size());

    for (const auto& pair : slots_) {
      buffer.push_back(pair.first);
    }

    for (const auto& id : buffer) {
      const auto it = slots_.find(id);
      if (it == slots_.end()) continue;

      it->second(arguments...);
    }
  }

private:
  std::unordered_map<int, Slot> slots_{};
};

template<typename... TArguments>
class Signal {
  using State = SignalState<TArguments...>;

public:
  using Slot = State::Slot;

  [[nodiscard]] SignalSubscription Subscribe(Slot slot) {
    auto id = state_->Subscribe(std::move(slot));
    return SignalSubscription(id, state_);
  }

  void Emit(TArguments... arguments) {
    state_->Emit(arguments...);
  }

private:
  std::shared_ptr<State> state_{std::make_shared<State>()};
};

}