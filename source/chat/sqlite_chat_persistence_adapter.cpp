#include "sqlite_chat_persistence_adapter.h"

namespace claw::chat::server {

void SqliteChatPersistenceAdapter::CreateChatChannel(const std::string &name) {
  //store_.Database().exec("")
}

void SqliteChatPersistenceAdapter::CreateChatMessage(std::int64_t channel_id, const std::string &message) {

}

}

