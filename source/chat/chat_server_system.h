#pragma once

#include "../application/application_system.h"
#include "../application/chat_tcp_system.h"
#include "chat_channel_store.h"
#include "chat_message_store.h"

#include <claw/core/system.h>

namespace claw::chat::server {

class ChatServerApplication;
class ChatPersistenceAdapter;
class UserServerSystem;
class ApplicationSystem;

class ChatServerSystem final : public core::System {
public:
  explicit ChatServerSystem(const core::SystemContext& ctx);

  void Initialize() override;
  void Deinitialize() override;

private:
  void HandleJoinChatChannel(network::ConnectionId id, const api::JoinChatChannelRequestMessage& message);
  void HandleWriteChatMessage(network::ConnectionId connection_id, const api::WriteChatMessage& message);
  void HandleCreateChatChannel(network::ConnectionId connection_id, const api::CreateChannelRequestMessage& message);
  void HandleShutdown(network::ConnectionId connection_id, const api::ShutdownMessage& message);
  void HandleGetChats(network::ConnectionId connection_id, const api::GetChatsRequestMessage& message);
  void HandleGetChatChannels(network::ConnectionId connection_id, const api::GetChatChannelsRequestMessage& message);

  //TODO code duplication, write helper utility, mixin or something
  template <typename TMessage>
  void RegisterMessageHandler(void(ChatServerSystem::*method)(network::ConnectionId, const TMessage&)) {
    app_system_.RegisterMessageHandler<TMessage>([this, method](network::ConnectionId id, const TMessage& message) {
      (this->*method)(id, message);
    });
  }

  ApplicationSystem& app_system_;
  ChatPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;
  UserServerSystem& user_system_;

  ChatChannelStore channel_store_{adapter_};
  ChatMessageStore message_store_{adapter_}; //TODO FIX: not fully thread safe while being accessed from multiple threads
};

}