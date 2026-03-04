#pragma once

#include <claw/core/application.h>

namespace claw::chat::server {

class ChatServerApplication final : public core::Application {
protected:
  void Initialize() override;
  void HandleTerminate() override;

};

}