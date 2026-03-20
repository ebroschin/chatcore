#include "chat_channel_message_log.h"

#include "adapters/chat_persistence_adapter.h"
#include "chat_message_store.h"

#include <algorithm>
#include <limits>

namespace claw::chat::server {

ChatChannelMessageLog::ChatChannelMessageLog(api::PersistenceId channel_id, ChatMessageStore& store, ChatPersistenceAdapter& adapter):
  channel_id_(channel_id),
  store_(store),
  adapter_(adapter)
{}

void ChatChannelMessageLog::Prewarm() const {
  constexpr auto max_message_id = std::numeric_limits<api::PersistenceId>::max();
  std::vector<api::ChatMessage> messages = adapter_.GetChatMessagesBefore(channel_id_, max_message_id, 100); //TODO constant
  if (messages.empty()) return;

  store_.CacheMessages(channel_id_, std::move(messages));
}

void ChatChannelMessageLog::AppendMessageId(api::PersistenceId message_id) {
  if (!channel_message_ids_.empty() && message_id <= channel_message_ids_.back()) return;
  channel_message_ids_.push_back(message_id);
}

void ChatChannelMessageLog::AssignMessageIds(std::vector<api::PersistenceId> message_ids) {
  std::ranges::sort(message_ids);
  message_ids.erase(std::ranges::unique(message_ids).begin(), message_ids.end());

  std::vector<api::PersistenceId> result;
  result.reserve(channel_message_ids_.size() + message_ids.size());

  std::ranges::merge(channel_message_ids_, message_ids, std::back_inserter(result));
  result.erase(std::ranges::unique(result).begin(), result.end());

  channel_message_ids_.swap(result);
}

std::vector<api::PersistenceId> ChatChannelMessageLog::GetLatestChatMessages(std::uint32_t limit) {
  constexpr auto max_message_id = std::numeric_limits<api::PersistenceId>::max();
  return GetChatMessagesBefore(max_message_id, limit);
}

std::vector<api::PersistenceId> ChatChannelMessageLog::GetChatMessagesBefore(api::PersistenceId message_id, std::uint32_t limit) {
  if (limit == 0) return {};

  //case 1: none of the requested data is available in cache
  if (channel_message_ids_.empty() || message_id <= channel_message_ids_.front()) {
    return QueryAndCacheMessages(message_id, limit);
  }

  //case 2: at least some of the requested data is available in cache
  const auto end_iterator = std::ranges::lower_bound(channel_message_ids_, message_id);
  auto cached_available_count = std::distance(channel_message_ids_.begin(), end_iterator);
  if (cached_available_count == 0) return QueryAndCacheMessages(message_id, limit);

  const auto limit_diff_t = static_cast<decltype(cached_available_count)>(limit);
  const auto cached_take_count = std::min(cached_available_count, limit_diff_t);

  std::vector<api::PersistenceId> cached_result;
  cached_result.reserve(static_cast<size_t>(cached_take_count));

  auto start_iterator = end_iterator;
  std::advance(start_iterator, -cached_take_count);
  std::ranges::copy(start_iterator, end_iterator, std::back_inserter(cached_result));

  if (cache_complete_ || limit_diff_t <= cached_take_count) {
    return cached_result;
  }

  //case 3: some of the requested data is only available in persistent storage
  const auto limit_remaining = static_cast<std::uint32_t>(limit_diff_t - cached_take_count);
  const auto oldest_cached_message_id = *start_iterator;

  std::vector<api::PersistenceId> persistence_result;
  auto remaining_messages = QueryAndCacheMessages(oldest_cached_message_id, limit_remaining);
  std::ranges::copy(remaining_messages, std::back_inserter(persistence_result));
  std::ranges::copy(cached_result, std::back_inserter(persistence_result));

  return persistence_result;
}

std::vector<api::PersistenceId> ChatChannelMessageLog::QueryAndCacheMessages(api::PersistenceId message_id, std::uint32_t limit) {
  std::vector<api::ChatMessage> messages = adapter_.GetChatMessagesBefore(channel_id_, message_id, limit);
  std::vector<api::PersistenceId> result;
  result.reserve(messages.size());

  for (const auto& message : messages) {
    result.push_back(message.id);
  }

  cache_complete_ = cache_complete_ || limit > messages.size();
  store_.CacheMessages(channel_id_, std::move(messages));
  return result;
}

}