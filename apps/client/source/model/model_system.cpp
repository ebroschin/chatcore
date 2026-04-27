#include "model_system.hpp"

#include <ebroschin/core/system_context.hpp>

namespace ebroschin::chatcore::client {

ModelSystem::ModelSystem(const core::SystemContext& ctx) noexcept:
  System{ctx}
{ }

void ModelSystem::AddLine(const std::string& line) {
  line_added_signal_.Emit(line);
}

void ModelSystem::SetChannelName(std::optional<std::string> channel_name) {
  channel_name_changed_signal_.Emit(std::move(channel_name));
}

utility::SignalSubscription ModelSystem::OnLineAdded(utility::Signal<const std::string&>::Slot slot) noexcept {
  return line_added_signal_.Subscribe(std::move(slot));
}

utility::SignalSubscription ModelSystem::OnChannelNameChanged(utility::Signal<const std::optional<std::string>>::Slot slot) noexcept {
  return channel_name_changed_signal_.Subscribe(std::move(slot));
}

}