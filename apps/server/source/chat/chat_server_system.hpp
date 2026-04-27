#pragma once

#include <ebroschin/core/system.hpp>
#include <ebroschin/scheduling/scheduling_system.hpp>

#include "../application/application_system.hpp"
#include "../application/chat_tcp_system.hpp"
#include "chat_channel_store.hpp"
#include "chat_message_store.hpp"

namespace ebroschin::chatcore::server {

class ChatServerApplication;
class ChatPersistenceAdapter;
class UserServerSystem;
class ApplicationSystem;

class ChatServerSystem final : public core::System {
public:
  explicit ChatServerSystem(const core::SystemContext& ctx);

  void Initialize() override;
  void Deinitialize() override;

  template <typename TMessage>
  void ChannelBroadcast(const api::ChatChannel& channel, const TMessage& message) {
    const auto channel_connections = channel_store_.GetConnections(channel.id);
    if (!channel_connections) return;

    tcp_system_.Broadcast<TMessage>(*channel_connections, message);
  }

  template <typename TMessage>
  void ChannelBroadcast(network::ConnectionId connection_id, const TMessage& message) {
    const auto potential_channel = channel_store_.GetAssignedChannel(connection_id);
    if (!potential_channel) return;

    ChannelBroadcast(potential_channel->get(), message);
  }

private:
  void HandleJoinChatChannel(network::ConnectionId id, const api::JoinChatChannelRequestMessage& message);
  void HandleWriteChatMessage(network::ConnectionId connection_id, const api::WriteChatMessage& message);
  void HandleCreateChatChannel(network::ConnectionId connection_id, const api::CreateChannelRequestMessage& message);
  void HandleShutdown(network::ConnectionId connection_id, const api::ShutdownMessage& message);
  void HandleLogout(network::ConnectionId, const api::LogoutRequestMessage&);
  void HandleGetChats(network::ConnectionId connection_id, const api::GetChatsRequestMessage& message);
  void HandleGetChatChannels(network::ConnectionId connection_id, const api::GetChatChannelsRequestMessage& message);
  void HandleGetChatChannel(network::ConnectionId connection_id, const api::GetChatChannelRequestMessage& message);

  ApplicationSystem& app_system_;
  ChatPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;
  UserServerSystem& user_system_;
  scheduling::SchedulingSystem& scheduling_system_;

  scheduling::TaskId message_persistence_task_{};
  ChatChannelStore channel_store_{adapter_};
  ChatMessageStore message_store_{adapter_};
};

}