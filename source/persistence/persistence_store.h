#pragma once

namespace claw::persistence {

class PersistenceStore {
public:
  virtual ~PersistenceStore() = default;

  virtual void Initialize() = 0;
  virtual void Deinitialize() = 0;
};

}