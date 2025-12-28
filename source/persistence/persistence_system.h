#pragma once

#include "persistence_adapter.h"

#include <claw/core/system.h>
#include <memory>

#include "../chat/sqlite_chat_persistence_adapter.h"
#include "persistence_store.h"

namespace claw::persistence {

class PersistenceSystemBase : public core::System {
public:
  explicit PersistenceSystemBase(const core::SystemContext& ctx)
    : System(ctx)
  {}

  void Initialize() override;
  void Deinitialize() override;

  template <typename TAdapter>
  requires std::derived_from<TAdapter, PersistenceAdapter>
  TAdapter* GetAdapter();

protected:
  virtual PersistenceAdapter* GetAdapter() = 0;
  virtual PersistenceStore* GetStore() = 0;
};

template <typename TAdapter>
requires std::derived_from<TAdapter, PersistenceAdapter>
TAdapter* PersistenceSystemBase::GetAdapter() {
  return static_cast<TAdapter*>(GetAdapter());
}

template<typename TPersistenceStore>
requires std::derived_from<TPersistenceStore, PersistenceStore>
class PersistenceSystem final : public PersistenceSystemBase {
public:
  template <typename... TArgs>
  explicit PersistenceSystem(const core::SystemContext& ctx, TArgs&&... args)
    : PersistenceSystemBase(ctx), store_{std::make_unique<TPersistenceStore>(std::forward<TArgs>(args)...)}
  {}

protected:
  PersistenceAdapter* GetAdapter() override {
    return test_.get();
  }

  PersistenceStore* GetStore() override {
    return store_.get();
  }

private:
  std::unique_ptr<TPersistenceStore> store_;

  //TODO registry
  std::unique_ptr<PersistenceAdapter> test_{std::make_unique<PersistenceAdapter>(dynamic_cast<TPersistenceStore*>(store_.get()))};
};

}