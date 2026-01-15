#pragma once
#include "persistence_store.h"

#include <concepts>
#include <claw/utility/concepts.h>

namespace claw::persistence {

class PersistenceStore;

class PersistenceAdapterBase {
public:
  virtual ~PersistenceAdapterBase() = default;
  virtual void Initialize() = 0;
  virtual void Deinitialize() = 0;
};

template<typename TPersistenceStore, typename TAdapterInterface>
requires std::derived_from<TPersistenceStore, PersistenceStore>
  && utility::VirtuallyDerivedFrom<TAdapterInterface, PersistenceAdapterBase>
class PersistenceAdapter : public virtual PersistenceAdapterBase, public TAdapterInterface {
public:
  explicit PersistenceAdapter(TPersistenceStore& store):
    store_{store}
  {}

protected:
  TPersistenceStore& store_;
};

}