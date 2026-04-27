#pragma once

namespace ebroschin::persistence {

class PersistenceStore {
public:
  virtual ~PersistenceStore() = default;

  virtual void Initialize() = 0;
  virtual void Deinitialize() {}
};

}