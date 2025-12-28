#pragma once
#include "persistence_store.h"

#include <concepts>

namespace claw::persistence {

class PersistenceStore;

class PersistenceAdapterBase {
public:
  virtual ~PersistenceAdapterBase() = default;
};

template<typename TPersistenceStore, typename TAdapterInterface>
requires std::derived_from<TPersistenceStore, PersistenceStore>
&& std::derived_from<TAdapterInterface, PersistenceAdapterBase>
class PersistenceAdapter : public virtual PersistenceAdapterBase, public TAdapterInterface {
public:
  explicit PersistenceAdapter(TPersistenceStore& store):
    store_{store}
  {}

protected:
  TPersistenceStore& store_;
};

}