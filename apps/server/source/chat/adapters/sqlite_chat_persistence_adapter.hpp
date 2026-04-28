#pragma once

#include "chat_persistence_adapter.hpp"

#include <ebroschin/persistence-modules/sqlite/sqlite_persistence_store.hpp>
#include <ebroschin/persistence/persistence_adapter.hpp>

#include <vector>

using namespace ebroschin::persistence::modules::sqlite;

namespace ebroschin::chatcore::server {

class SqliteChatPersistenceAdapter final : public persistence::PersistenceAdapter<SqlitePersistenceStore, ChatPersistenceAdapter> {
public:
  explicit SqliteChatPersistenceAdapter(SqlitePersistenceStore& store) noexcept;

  void Initialize() override;

  std::optional<api::ChatChannel> CreateChatChannel(const std::string &name) override;
  std::optional<api::ChatChannel> GetChatChannel(const std::string& name) override;
  std::optional<api::ChatChannel> GetChatChannel(api::PersistenceId id) override;
  std::vector<api::ChatChannel> GetChatChannels() override;
  std::optional<api::PersistenceId> PersistChatMessages(std::span<const api::ChatMessage> messages) override;

  std::optional<api::ChatMessage>
  CreateChatMessage(api::PersistenceId channel_id, api::PersistenceId user_id, const std::string& content) override;

  std::vector<api::ChatMessage>
  GetChatMessagesBefore(api::PersistenceId channel_id, api::PersistenceId message_id, std::uint32_t limit) override;

  std::optional<api::PersistenceId> GetFirstChatMessageId(api::PersistenceId channel_id) override;
  std::optional<api::PersistenceId> GetLastChatMessageId() override;
};

}