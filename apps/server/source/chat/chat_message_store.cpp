#include "chat_message_store.hpp"

#include "adapters/chat_persistence_adapter.hpp"

namespace ebroschin::chatcore::server {

ChatMessageStore::ChatMessageStore(ChatPersistenceAdapter& adapter) noexcept:
  adapter_{adapter}
{}

void ChatMessageStore::Prewarm() {
  {
    std::scoped_lock lock{mutex_};
    latest_persisted_id_ = adapter_.GetLastChatMessageId();
    next_id_ = latest_persisted_id_? *latest_persisted_id_ + 1 : 0;
  }

  const auto channels = adapter_.GetChatChannels();
  for (const auto& channel : channels) {
    auto [it, _] = message_logs_.try_emplace(channel.id, channel.id, *this, adapter_);
    it->second.Prewarm();
  }
}

void ChatMessageStore::CreateMessage(api::PersistenceId channel_id, api::PersistenceId user_id, const std::string& message) {
  const auto id = next_id_++;
  CacheMessage({id, channel_id, user_id, message});
}

void ChatMessageStore::CacheMessage(api::ChatMessage chat_message) {
  auto channel_id = chat_message.channel_id;
  auto [it, _] = message_logs_.try_emplace(channel_id, channel_id, *this, adapter_);
  it->second.AppendMessageId(chat_message.id);
  AssignMessage(std::move(chat_message));
}

void ChatMessageStore::CacheMessages(api::PersistenceId channel_id, std::vector<api::ChatMessage> chat_messages) {
  if (chat_messages.empty()) return;

  std::vector<api::PersistenceId> buffer{};
  buffer.reserve(chat_messages.size());

  for (auto& chat_message : chat_messages) {
    buffer.emplace_back(chat_message.id);
    AssignMessage(std::move(chat_message));
  }

  const auto [it, _] = message_logs_.try_emplace(channel_id, channel_id, *this, adapter_);
  it->second.AssignMessageIds(std::move(buffer));
}

void ChatMessageStore::AssignMessage(api::ChatMessage chat_message) {
  const auto message_id = chat_message.id;
  const auto [it, _] = message_cache_.insert_or_assign(message_id, std::move(chat_message));

  {
    std::scoped_lock lock{mutex_};
    if (latest_persisted_id_ && message_id <= *latest_persisted_id_) return;

    pending_messages_.emplace_back(it->second);
  }
}

void ChatMessageStore::Persist() {
  std::vector<api::ChatMessage> messages{};
  {
    std::scoped_lock lock{mutex_};
    if (pending_messages_.empty()) return;

    pending_messages_.swap(messages);
  }

  const auto latest_persisted_id = adapter_.PersistChatMessages(messages);
  {
    std::scoped_lock lock{mutex_};
    latest_persisted_id_ = latest_persisted_id;
  }
}

std::optional<std::reference_wrapper<const api::ChatMessage>>
ChatMessageStore::GetMessage(api::PersistenceId message_id) {
  const auto it = message_cache_.find(message_id);
  if (it == message_cache_.end()) return std::nullopt;

  return it->second;
}

std::vector<api::ChatMessage>
ChatMessageStore::GetMessagesBefore(api::PersistenceId channel_id, api::PersistenceId message_id, std::uint32_t limit) {
  const auto it = message_logs_.find(channel_id);
  if (it == message_logs_.end()) return {};

  std::vector<api::ChatMessage> result{};
  const auto ids = it->second.GetChatMessagesBefore(message_id, limit);
  for (const auto id : ids) {
    const auto message = GetMessage(id);
    if (!message) continue;

    result.push_back(*message);
  }

  return result;
}

}