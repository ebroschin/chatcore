#pragma once

#include <claw/core/application.h>

namespace claw::chat::client {

class ChatClientApplication final : public core::Application {
public:
  void Initialize() override;
  void HandleTerminate() override;

};

}