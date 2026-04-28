#include "chat_channel_message_log.hpp"

#include "adapters/chat_persistence_adapter.hpp"
#include "chat_message_store.hpp"

#include <algorithm>
#include <limits>

namespace ebroschin::chatcore::server {

ChatChannelMessageLog::ChatChannelMessageLog(api::PersistenceId channel_id,
  ChatMessageStore& store,
  ChatPersistenceAdapter& adapter) noexcept:
  channel_id_{channel_id},
  store_{store},
  adapter_{adapter}
{}

void ChatChannelMessageLog::Prewarm() const {
  constexpr auto max_message_id = std::numeric_limits<api::PersistenceId>::max();
  auto messages = adapter_.GetChatMessagesBefore(channel_id_, max_message_id, PrewarmMessageCount);
  if (messages.empty()) return;

  store_.CacheMessages(channel_id_, std::move(messages));
}

void ChatChannelMessageLog::AppendMessageId(api::PersistenceId message_id) {
  if (!channel_message_ids_.empty() && message_id <= channel_message_ids_.back()) return;
  channel_message_ids_.push_back(message_id);
}

void ChatChannelMessageLog::AssignMessageIds(std::vector<api::PersistenceId> message_ids) {
  if (message_ids.empty()) return;

  //sort and deduplicate incoming ids
  std::ranges::sort(message_ids);
  message_ids.erase(std::ranges::unique(message_ids).begin(), message_ids.end());

  //insert incoming message ids
  channel_message_ids_.reserve(channel_message_ids_.size() + message_ids.size());
  channel_message_ids_.insert(channel_message_ids_.end(), message_ids.begin(), message_ids.end());

  //perform inplace merge to guarantee global sorting
  const auto message_count = static_cast<decltype(channel_message_ids_)::difference_type>(message_ids.size());
  std::ranges::inplace_merge(channel_message_ids_, channel_message_ids_.end() - message_count);

  //assure deduplication
  channel_message_ids_.erase(std::ranges::unique(channel_message_ids_).begin(), channel_message_ids_.end());
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

  std::vector<api::PersistenceId> cached_result{};
  cached_result.reserve(static_cast<size_t>(cached_take_count));

  auto start_iterator = end_iterator;
  std::advance(start_iterator, -cached_take_count);
  cached_result.insert(cached_result.end(), start_iterator, end_iterator);

  if (cache_complete_ || limit_diff_t <= cached_take_count) {
    return cached_result;
  }

  //case 3: some of the requested data is only available in persistent storage
  const auto limit_remaining = static_cast<std::uint32_t>(limit_diff_t - cached_take_count);
  const auto oldest_cached_message_id = *start_iterator;

  std::vector<api::PersistenceId> persistence_result{};
  persistence_result.reserve(limit_remaining + cached_result.size());

  const auto remaining_messages = QueryAndCacheMessages(oldest_cached_message_id, limit_remaining);
  persistence_result.insert(persistence_result.end(), remaining_messages.begin(), remaining_messages.end());
  persistence_result.insert(persistence_result.end(), cached_result.begin(), cached_result.end());

  return persistence_result;
}

std::vector<api::PersistenceId> ChatChannelMessageLog::QueryAndCacheMessages(api::PersistenceId message_id, std::uint32_t limit) {
  auto messages = adapter_.GetChatMessagesBefore(channel_id_, message_id, limit);
  std::vector<api::PersistenceId> result{};
  result.reserve(messages.size());

  for (const auto& message : messages) {
    result.push_back(message.id);
  }

  cache_complete_ = cache_complete_ || limit > messages.size();
  store_.CacheMessages(channel_id_, std::move(messages));
  return result;
}

}