#pragma once
#include "../persistence/persistence_store.h"

#include <string>
#include <SQLiteCpp/SQLiteCpp.h>

namespace claw::persistence::sqlite {

class SqlitePersistenceStore final : public PersistenceStore {
public:
  explicit SqlitePersistenceStore(const std::string& file_name)
    : db_{file_name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE}
  {}

  void Initialize() override;
  void Deinitialize() override;

  SQLite::Database& Database() { return db_; }
private:
  SQLite::Database db_;
};

}