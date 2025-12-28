#pragma once

#include "persistence_adapter.h"

#include <claw/core/system.h>
#include <memory>
#include "persistence_store.h"

#include <typeindex>
#include <unordered_map>

namespace claw::persistence {

class PersistenceSystemBase : public core::System {
public:
  explicit PersistenceSystemBase(const core::SystemContext& ctx):
    System(ctx)
  {}

  template <typename TAdapterInterface>
  TAdapterInterface* Get() {
    return dynamic_cast<TAdapterInterface*>(GetAdapter(typeid(TAdapterInterface)));
  }

protected:
  virtual PersistenceAdapterBase* GetAdapter(const std::type_index& key) = 0;
};

template <typename TStore>
requires std::derived_from<TStore, PersistenceStore>
class PersistenceSystem final : public PersistenceSystemBase {
public:
  template <typename... TArgs>
  explicit PersistenceSystem(const core::SystemContext& ctx, TArgs&&... args)
    : PersistenceSystemBase(ctx)
    , store_(std::make_unique<TStore>(std::forward<TArgs>(args)...))
  {}

  void Initialize() override {
    store_->Initialize();
  }

  void Deinitialize() override {
    store_->Deinitialize();
  }

  template <typename TAdapterInterface, typename TAdapter, typename... TArgs>
  requires std::derived_from<TAdapter, TAdapterInterface> &&
    std::derived_from<TAdapter, PersistenceAdapter<TStore, TAdapterInterface>>
  void Register(TArgs&&... args) {
    std::type_index key = typeid(TAdapterInterface);
    auto ptr = std::make_unique<TAdapter>(*store_, std::forward<TArgs>(args)...);
    adapters_.emplace(key, std::move(ptr));
  }

protected:
  PersistenceAdapterBase* GetAdapter(const std::type_index& key) override {
    const auto it = adapters_.find(key);
    return it != adapters_.end()? it->second.get() : nullptr;
  }

private:
  std::unique_ptr<TStore> store_;
  std::unordered_map<std::type_index, std::unique_ptr<PersistenceAdapterBase>> adapters_;
};

}