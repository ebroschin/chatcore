#pragma once

#include "tcp_system.h"

namespace claw::communication {

template<typename TConnection, typename TSerializer, typename TMessageTuple>
struct TcpSystemParameters {
  using Type = TcpSystem<TConnection, TSerializer, TMessageTuple>;
};

template<typename TConnection, typename TSerializer, typename... TMessages>
struct TcpSystemParameters<TConnection, TSerializer, std::tuple<TMessages...>> {
  using Type = TcpSystem<TConnection, TSerializer, TMessages...>;
};

}