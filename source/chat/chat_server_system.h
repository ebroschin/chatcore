#pragma once

#include "../application/chat_tcp_system.h"
#include "chat_channel_store.h"
#include "chat_message_store.h"

#include <claw/core/system.h>
#include <string>

namespace claw::chat::server {

class ChatPersistenceAdapter;
class UserServerSystem;

class ChatServerSystem final : public core::System {
public:
  explicit ChatServerSystem(const core::SystemContext& ctx);

  void Initialize() override;

  void JoinChatChannel(network::ConnectionId id, api::PersistenceId channel_id);
  void WriteChatMessage(network::ConnectionId connection_id, const std::string& content);
  void CreateChatChannel(network::ConnectionId connection_id, const std::string& name);

private:
  ChatPersistenceAdapter& adapter_;
  ChatServerTcpSystem& tcp_system_;

  UserServerSystem* user_system_{};

  //this is state. this will need to be thread safe as soon as multiple message processors perform actions on this object
  ChatChannelStore channel_store_{adapter_};
  ChatMessageStore message_store_{adapter_};
};

}