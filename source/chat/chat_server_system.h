#pragma once

#include "../persistence/persistence_system.h"

#include <claw/core/system.h>
#include <string>
#include <vector>

namespace claw::communication {
class TcpSystem;
}
namespace claw::chat::server {

class ChatPersistenceAdapter;

class ChatServerSystem final : public core::System {
public:
  explicit ChatServerSystem(const core::SystemContext& ctx);

  void CreateChatChannel(const std::string& name);
  void CreateChatMessage(std::int64_t channel_id, const std::string& message);
  std::vector<std::string> GetChatMessages(std::int64_t channel_id);

  [[nodiscard]] communication::TcpSystem& GetTcpSystem() {
    return tcp_system_;
  }
  
private:
  ChatPersistenceAdapter& adapter_;
  communication::TcpSystem& tcp_system_;
};

}