#include "chat_input_system.h"

#include <iostream>

namespace claw::chat::client {

void ChatInputSystem::Initialize() {
  running_ = true;
}

void ChatInputSystem::Deinitialize() {
  running_ = false;
  if (!worker_.joinable()) return;

  worker_.join();
}

void ChatInputSystem::UpdateWorker() {
  std::string line;
  while (running_ && std::getline(std::cin, line)) {
    std::lock_guard lock{mutex_};
    queue_.push(line);
  }
}

}