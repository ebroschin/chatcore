#pragma once

#include <ebroschin/persistence/persistence_store.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <string>

namespace ebroschin::persistence::modules::sqlite {

class SqlitePersistenceStore final : public PersistenceStore {
public:
  explicit SqlitePersistenceStore(const std::string& file_name)
    : db_{file_name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE}
  {}

  void Initialize() override;

  [[nodiscard]] SQLite::Database& GetDatabase() noexcept
  { return db_; }

private:
  SQLite::Database db_;
};

}