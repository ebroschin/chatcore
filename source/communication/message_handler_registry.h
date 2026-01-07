#pragma once

#include <functional>
#include <tuple>
#include "../utility/variadic.h"

namespace claw::communication {

template<typename... TMessages>
class MessageHandlerRegistry {
public:
  template<typename TMessage>
  void HandleMessage(ConnectionID id, const TMessage& message) {
    constexpr int index = utility::IndexOf<TMessage, TMessages...>();
    auto& handler = std::get<index>(handlers_);
    if (!handler) return;

    handler(id, message);
  }

  template<typename TMessage>
  void Register(std::function<void(ConnectionID, const TMessage&)> function) {
    constexpr int index = utility::IndexOf<TMessage, TMessages...>();
    std::get<index>(handlers_) = std::move(function);
  }

private:
  std::tuple<std::function<void(ConnectionID, const TMessages&)>...> handlers_{};
};

}