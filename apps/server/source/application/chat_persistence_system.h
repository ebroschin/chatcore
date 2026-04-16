#pragma once

#include "../persistence/persistence_system.h"
#include "../sqlite_persistence/sqlite_persistence_store.h"

namespace ebroschin::chatcore::server {

using ChatPersistenceSystem = persistence::PersistenceSystem<persistence::modules::sqlite::SqlitePersistenceStore>;

}