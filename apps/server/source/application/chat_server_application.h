#pragma once

#include <claw/core/application.h>
#include "chat_server_arguments.h"

namespace claw::chat::server {

class ChatServerApplication final : public core::Application {
public:
  explicit ChatServerApplication(ChatServerArguments arguments);

  [[nodiscard]] const ChatServerArguments& GetArguments() const noexcept
  { return arguments_; }

protected:
  void Initialize() override;
  void HandleTerminate() override;

  ChatServerArguments arguments_;
};

}