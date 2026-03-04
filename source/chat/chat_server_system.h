#pragma once

#include "../application/commons.h"
#include "chat_channel_store.h"
#include "chat_message_store.h"

#include <claw/core/system.h>
#include <string>

namespace claw::chat::server {

class ChatServerApplication;
class ChatPersistenceAdapter;
class UserServerSystem;

class ChatServerSystem final : public core::System {
public:
  explicit ChatServerSystem(const core::SystemContext& ctx, ChatServerApplication& app);

  void Initialize() override;
  void Deinitialize() override;

  void JoinChatChannel(network::ConnectionId id, network::RequestId request_id, api::PersistenceId channel_id);
  void WriteChatMessage(network::ConnectionId connection_id, const std::string& content);
  void CreateChatChannel(network::ConnectionId connection_id, network::RequestId request_id, const std::string& name) const;

private:
  ChatServerApplication& app_;
  ChatPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;
  UserServerSystem& user_system_;

  ChatChannelStore channel_store_{adapter_};
  ChatMessageStore message_store_{adapter_};
};

}