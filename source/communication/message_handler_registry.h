#pragma once

#include <functional>
#include <tuple>
#include "../utility/variadic.h"

namespace claw::communication {

template<typename... TMessages>
class MessageHandlerRegistry {
public:
  template<typename TMessage>
  void HandleMessage(const TMessage& message) {
    constexpr int index = utility::IndexOf<TMessage, TMessages...>();
    std::get<index>(handlers_)(message);
  }

  template<typename TMessage>
  void Register(std::function<void(const TMessage&)> function) {
    constexpr int index = utility::IndexOf<TMessage, TMessages...>();
    std::get<index>(handlers_) = std::move(function);
  }

private:
  std::tuple<std::function<void(const TMessages&)>...> handlers_{};
};

}