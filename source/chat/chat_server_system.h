#pragma once

#include "../persistence/persistence_system.h"

#include <claw/core/system.h>
#include <string>
#include <vector>

namespace claw::communication {
class TcpServerSystemBase;
}
namespace claw::chat::server {

class ChatPersistenceAdapter;

class ChatServerSystem final : public core::System {
public:
  explicit ChatServerSystem(const core::SystemContext& ctx);

  void CreateChatChannel(const std::string& name);
  void CreateChatMessage(std::int64_t channel_id, const std::string& message);
  std::vector<std::string> GetChatMessages(std::int64_t channel_id);

  [[nodiscard]] communication::TcpServerSystemBase& GetCommunicationSystem() {
    return tcp_server_system_;
  }
  
private:
  ChatPersistenceAdapter& adapter_;
  communication::TcpServerSystemBase& tcp_server_system_;
};

}