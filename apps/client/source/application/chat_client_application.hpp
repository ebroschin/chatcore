#pragma once

#include <ebroschin/core/application.hpp>

namespace ebroschin::chatcore::client {

class ChatClientApplication final : public core::Application {
public:
  void Initialize() override;
  void HandleTerminate() override;
};

}