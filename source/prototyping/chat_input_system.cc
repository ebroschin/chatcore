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
  while (true) {
    if (!running_) continue;
    std::getline(std::cin, line);
    std::lock_guard lock{mutex_};
    queue_.push(line);
  }
}

bool ChatInputSystem::GetLine(std::string& out)  {
  std::lock_guard lock{mutex_};
  if (queue_.empty()) return false;

  out = std::move(queue_.front());
  queue_.pop();

  return true;
}


}