#pragma once
#include <memory>


namespace claw::utility {

class SignalStateBase;

class SignalSubscription {
public:
  explicit SignalSubscription(int id, std::weak_ptr<SignalStateBase> signal_state);

  SignalSubscription() = default;
  ~SignalSubscription();

  SignalSubscription(const SignalSubscription&) = delete;
  void operator=(const SignalSubscription&) = delete;

  SignalSubscription(SignalSubscription&&) noexcept;
  SignalSubscription& operator=(SignalSubscription&&) noexcept;

  void Unsubscribe();

private:
  int id_{-1};
  std::weak_ptr<SignalStateBase> signal_state_{};
};

}