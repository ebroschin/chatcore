#include <claw/core/system_context.h>

#include <algorithm>
#include <ranges>

namespace claw::core {

void SystemContext::Initialize() {
  std::ranges::for_each(ordered_systems_, &System::Initialize);
}

void SystemContext::BeginFrame() const {
  std::ranges::for_each(ordered_systems_, &System::BeginFrame);
}

void SystemContext::Update() const {
  std::ranges::for_each(ordered_systems_, &System::Update);
}

void SystemContext::EndFrame() const {
  std::ranges::for_each(ordered_systems_, &System::EndFrame);
}

void SystemContext::Deinitialize() const {
  std::ranges::for_each(ordered_systems_ | std::views::reverse, &System::Deinitialize);
}

}