#pragma once

#include "../source/chat/adapters/chat_persistence_adapter.h"

#include <algorithm>
#include <cstdint>
#include <optional>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

namespace claw::chat::server::tests {

class MockChatPersistenceAdapter final : public ChatPersistenceAdapter {
public:
  std::optional<api::ChatChannel> CreateChatChannel(const std::string& name) override {
    const auto id = next_channel_id_++;

    api::ChatChannel channel{id, name};
    channels_by_id_.emplace(id, channel);
    channels_by_name_.emplace(name, id);

    return channel;
  }

  std::optional<api::ChatChannel> GetChatChannel(api::PersistenceId id) override {
    const auto it = channels_by_id_.find(id);
    if (it == channels_by_id_.end()) return std::nullopt;

    return it->second;
  }

  std::optional<api::ChatChannel> GetChatChannel(const std::string& name) override {
    const auto it = channels_by_name_.find(name);
    if (it == channels_by_name_.end()) return std::nullopt;

    return GetChatChannel(it->second);
  }

  std::vector<api::ChatChannel> GetChatChannels() override {
    auto view = channels_by_id_ | std::views::values;
    return std::vector<api::ChatChannel>{view.begin(), view.end()};
  }

  std::optional<api::PersistenceId> PersistChatMessages(std::span<const api::ChatMessage> messages) override {
    if (messages.empty()) return last_message_id_;

    for (const auto& message : messages) {
      auto channel_id = message.channel_id;
      messages_by_channel_[channel_id].push_back(message);
      messages_by_id_[message.id] = message;
      last_message_id_ = message.id;
    }

    return last_message_id_;
  }

  std::optional<api::ChatMessage> CreateChatMessage(api::PersistenceId channel_id, api::PersistenceId user_id, const std::string& content) override {
    const auto id = next_message_id_++;
    api::ChatMessage message{id, channel_id, user_id, content};
    messages_by_channel_[channel_id].push_back(message);
    messages_by_id_[id] = message;
    last_message_id_ = id;
    return message;
  }

  std::vector<api::ChatMessage> GetChatMessagesBefore(api::PersistenceId channel_id, api::PersistenceId message_id, std::uint32_t limit) override {
    if (limit == 0) return {};

    const auto it = messages_by_channel_.find(channel_id);
    if (it == messages_by_channel_.end()) return {};

    std::vector<api::ChatMessage> result;
    result.reserve(it->second.size());

    for (const auto& message : it->second) {
      if (message.id < message_id) result.emplace_back(message);
    }

    std::ranges::sort(result, {}, &api::ChatMessage::id);
    if (result.size() > limit) {
      result.erase(result.begin(), result.end() - limit);
    }

    return result;
  }

  std::optional<api::PersistenceId> GetFirstChatMessageId(api::PersistenceId channel_id) override {
    const auto it = messages_by_channel_.find(channel_id);
    if (it == messages_by_channel_.end()) return std::nullopt;
    if (it->second.empty()) return std::nullopt;

    const auto min_it = std::ranges::min_element(it->second, {}, &api::ChatMessage::id);
    return min_it->id;
  }

  std::optional<api::PersistenceId> GetLastChatMessageId() override {
    return last_message_id_;
  }

private:
  api::PersistenceId next_channel_id_{1};
  api::PersistenceId next_message_id_{1};

  std::unordered_map<api::PersistenceId, api::ChatChannel> channels_by_id_{};
  std::unordered_map<std::string, api::PersistenceId> channels_by_name_{};

  std::unordered_map<api::PersistenceId, std::vector<api::ChatMessage>> messages_by_channel_{};
  std::unordered_map<api::PersistenceId, api::ChatMessage> messages_by_id_{};

  std::optional<api::PersistenceId> last_message_id_{std::nullopt};

};

}

