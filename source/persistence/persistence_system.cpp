#include "persistence_system.h"

namespace claw::persistence {

void PersistenceSystemBase::Initialize() {
  GetStore()->Initialize();
}

void PersistenceSystemBase::Deinitialize() {
  GetStore()->Deinitialize();
}

}