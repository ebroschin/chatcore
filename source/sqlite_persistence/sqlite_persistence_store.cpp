#include "sqlite_persistence_store.h"

namespace claw::persistence::sqlite {

void SqlitePersistenceStore::Initialize() {
  db_.exec("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, value TEXT)");
}

void SqlitePersistenceStore::Deinitialize() {

}

}