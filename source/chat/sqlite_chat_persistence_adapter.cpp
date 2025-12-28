#include "sqlite_chat_persistence_adapter.h"

namespace claw::chat::server {

void SqliteChatPersistenceAdapter::CreateChatChannel(const std::string &name) {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_channels (name) VALUES (?);");
  query.bind(1, name);
  query.exec();
}

void SqliteChatPersistenceAdapter::CreateChatMessage(std::int64_t channel_id, const std::string &message) {
  SQLite::Statement query(store_.Database(), "INSERT INTO chat_messages (channel_id, message) VALUES (?,?);");
  query.bind(1, channel_id);
  query.bind(2, message);
  query.exec();
}

}

