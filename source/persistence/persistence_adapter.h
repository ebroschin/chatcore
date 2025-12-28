#pragma once
#include "persistence_store.h"

#include <concepts>

namespace claw::persistence {

class PersistenceStore;

// template<typename TPersistenceStore>
// requires std::derived_from<TPersistenceStore, PersistenceStore>
class PersistenceAdapter {
public:
  explicit PersistenceAdapter(PersistenceStore* store)
    : store_(store)
  {}

protected:
  PersistenceStore* store_;
};

}