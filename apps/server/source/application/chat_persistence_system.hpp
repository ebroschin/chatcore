#pragma once

#include <ebroschin/persistence-modules/sqlite/sqlite_persistence_store.hpp>
#include <ebroschin/persistence/persistence_system.hpp>

namespace ebroschin::chatcore::server {

using ChatPersistenceSystem = persistence::PersistenceSystem<persistence::modules::sqlite::SqlitePersistenceStore>;

}