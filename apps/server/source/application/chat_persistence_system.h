#pragma once

#include <ebroschin/persistence-modules/sqlite/sqlite_persistence_store.h>
#include <ebroschin/persistence/persistence_system.h>

namespace ebroschin::chatcore::server {

using ChatPersistenceSystem = persistence::PersistenceSystem<persistence::modules::sqlite::SqlitePersistenceStore>;

}