#include "chat_message_store.h"

#include "adapters/chat_persistence_adapter.h"

#include <iostream>

namespace claw::chat::server {

ChatMessageStore::ChatMessageStore(ChatPersistenceAdapter& adapter):
  adapter_(adapter)
{ }

void ChatMessageStore::Prewarm() {
  latest_persisted_id_ = adapter_.GetLastChatMessageId();
  next_id_ = latest_persisted_id_.has_value()? *latest_persisted_id_ + 1 : 0;

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

  //TODO code duplication
  message_cache_.insert_or_assign(chat_message.id, std::move(chat_message));
  if (latest_persisted_id_ && chat_message.id <= *latest_persisted_id_) return;
  pending_message_ids_.push(chat_message.id);
}

void ChatMessageStore::CacheMessages(api::PersistenceId channel_id, std::vector<api::ChatMessage> chat_messages) {
  std::vector<api::PersistenceId> buffer{};
  buffer.reserve(chat_messages.size());

  for (auto& chat_message : chat_messages) {
    buffer.push_back(chat_message.id);

    //TODO code duplication
    message_cache_.insert_or_assign(chat_message.id, std::move(chat_message));
    if (latest_persisted_id_ && chat_message.id <= *latest_persisted_id_) continue;
    pending_message_ids_.push(chat_message.id);
  }

  auto [it, _] = message_logs_.try_emplace(channel_id, channel_id, *this, adapter_);
  it->second.AssignMessageIds(std::move(buffer));
}

void ChatMessageStore::Persist() {
  thread_local std::vector<api::ChatMessage> messages;
  messages.clear();

  std::unique_lock lock(persist_mutex_);
  if (pending_message_ids_.empty()) return;

  messages.reserve(pending_message_ids_.size());

  while (!pending_message_ids_.empty()) {
    const auto id = pending_message_ids_.front();
    pending_message_ids_.pop();

    auto message = GetMessage(id);
    if (!message) continue;

    messages.push_back(*message);
  }

  lock.unlock();
  std::cout << "persisting" << std::endl;
  const auto latest_persisted_id = adapter_.PersistChatMessages(messages);
  lock.lock();

  latest_persisted_id_ = latest_persisted_id;
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

  std::vector<api::ChatMessage> result;
  const auto ids = it->second.GetChatMessagesBefore(message_id, limit);
  for (const auto id : ids) {
    const auto message = GetMessage(id);
    if (!message) continue;

    result.push_back(*message);
  }

  return result;
}

}