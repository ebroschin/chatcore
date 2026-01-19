#include "chat_input_system.h"

#include "client_test_system.h"

#include <iostream>
#include <string>
#include <claw/core/system_context.h>

namespace claw::chat::client {

void ChatInputSystem::Initialize() {
  running_ = true;
  worker_ = std::thread{&ChatInputSystem::UpdateWorker, this};
}

void ChatInputSystem::Deinitialize() {
  running_ = false;
  if (!worker_.joinable()) return;

  worker_.join();
}

void ChatInputSystem::UpdateWorker() {
  std::string line;
  while (running_) {
    std::getline(std::cin, line);
    ctx_.Get<prototyping::ClientTestSystem>()->HandleLine(line);
  }
}

}