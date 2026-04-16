#include "sqlite_persistence_store.h"

namespace ebroschin::persistence::modules::sqlite {

void SqlitePersistenceStore::Initialize() {
  db_.exec("PRAGMA foreign_keys=ON;");
  db_.exec("PRAGMA synchronous=NORMAL;");
  db_.exec("PRAGMA journal_mode=WAL;");
}

}