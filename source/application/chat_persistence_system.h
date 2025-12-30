#pragma once

#include "../persistence/persistence_system.h"
#include "../sqlite_persistence/sqlite_persistence_store.h"

namespace claw::chat::server {
using ChatPersistenceSystem = persistence::PersistenceSystem<persistence::sqlite::SqlitePersistenceStore>;
}