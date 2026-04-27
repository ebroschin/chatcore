#pragma once

#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "system.hpp"

namespace ebroschin::core {

class SystemContext {
public:
  template <typename TSystem, typename... Args>
  requires std::derived_from<TSystem, System>
  TSystem* Register(Args&&... arguments);

  template <typename TSystemInterface, typename TSystem, typename... Args>
  requires std::derived_from<TSystem, TSystemInterface>
    && std::derived_from<TSystemInterface, System>
  TSystemInterface* Register(Args&&... arguments);

  template<typename TSystem>
  TSystem* Get() const;

  template<typename TSystem>
  TSystem& Require() const;

  void Initialize();
  void Deinitialize() const;

private:
  std::unordered_map<std::type_index, std::unique_ptr<System>> systems_{};
  std::vector<System*> ordered_systems_{};
};

template <typename TSystem, typename... TArgs>
requires std::derived_from<TSystem, System>
TSystem* SystemContext::Register(TArgs&&... arguments) {
  return Register<TSystem, TSystem>(std::forward<TArgs>(arguments)...);
}

template <typename TSystemInterface, typename TSystem, typename... Args>
requires std::derived_from<TSystem, TSystemInterface>
  && std::derived_from<TSystemInterface, System>
TSystemInterface* SystemContext::Register(Args&&... arguments) {
  const std::type_index key = typeid(TSystemInterface);
  auto system = std::make_unique<TSystem>(*this, std::forward<Args>(arguments)...);
  auto [it, _] = systems_.emplace(key, std::move(system));

  auto* ptr = static_cast<TSystemInterface*>(it->second.get());
  ordered_systems_.emplace_back(ptr);
  return ptr;
}

template<typename TSystem>
TSystem* SystemContext::Get() const {
  const std::type_index key = typeid(TSystem);
  const auto it = systems_.find(key);
  return it != systems_.end()? static_cast<TSystem*>(it->second.get()) : nullptr;
}

template<typename TSystem>
TSystem& SystemContext::Require() const {
  auto* system = Get<TSystem>();
  if (!system) {
    std::cerr << "Required System not registered" << std::endl;
    std::abort();
  }

  return *system;
}

}