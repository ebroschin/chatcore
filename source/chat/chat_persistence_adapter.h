#pragma once

#include <concepts>
#include "../persistence/persistence_adapter.h"

#include <string>

namespace claw::persistence {
class PersistenceStore;
}

namespace claw::chat::server {

// class ChatPersistenceAdapterBase : public persistence::PersistenceAdapterBase {
//   virtual void CreateChatMessage(std::int64_t channel_id, const std::string& message) = 0;
//   virtual void CreateChatChannel(const std::string& name) = 0;
// };

// template<typename TPersistenceStore>
// requires std::derived_from<TPersistenceStore, persistence::PersistenceStore>
class ChatPersistenceAdapter : public persistence::PersistenceAdapter {
public:
  explicit ChatPersistenceAdapter(persistence::PersistenceStore* store)
  : PersistenceAdapter(store)
  {}

  virtual void CreateChatMessage(std::int64_t channel_id, const std::string& message) = 0;
  virtual void CreateChatChannel(const std::string& name) = 0;
};

}