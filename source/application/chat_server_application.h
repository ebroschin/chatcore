#pragma once

#include <claw/core/application.h>

namespace claw::chat::server {

class ChatServerApplication final : public core::Application {
public:
  explicit ChatServerApplication(const char* argument)
    : argument_{argument}
  {}

protected:
  void HandleTerminate() override;
  void Initialize() override;

private:
  std::string argument_;
};

}