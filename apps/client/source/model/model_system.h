#pragma once

#include <claw/core/system.h>
#include <claw/utility/signal.h>

#include <vector>
#include <string>
#include <span>

namespace claw::chat::client {

class SessionSystem;

class ModelSystem final : public core::System {
public:
  using ChatLogView = std::span<const std::string>;

  explicit ModelSystem(const core::SystemContext& ctx) noexcept;

  void AddLine(const std::string& line);
  void SetChannelName(std::optional<std::string> channel_name);

  [[nodiscard]] utility::SignalSubscription OnLineAdded(utility::Signal<const std::string&>::Slot slot) noexcept;
  [[nodiscard]] utility::SignalSubscription OnChannelNameChanged(utility::Signal<const std::optional<std::string>>::Slot slot) noexcept;

private:
  utility::Signal<ChatLogView> chat_log_signal_{};
  utility::Signal<const std::string&> line_added_signal_{};
  utility::Signal<const std::optional<std::string>> channel_name_changed_signal_{};
};

}