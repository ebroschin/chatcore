#pragma once

#include <ebroschin/core/system.hpp>
#include <ebroschin/utility/signal.hpp>

#include <span>
#include <string>

namespace ebroschin::chatcore::client {

class ModelSystem final : public core::System {
public:
  using ChatLogView = std::span<const std::string>;

  explicit ModelSystem(const core::SystemContext& ctx) noexcept;

  void AddLine(const std::string& line);
  void SetChannelName(std::optional<std::string> channel_name);

  [[nodiscard]] utility::SignalSubscription
  OnLineAdded(utility::Signal<const std::string&>::Slot slot) noexcept;

  [[nodiscard]] utility::SignalSubscription
  OnChannelNameChanged(utility::Signal<const std::optional<std::string>>::Slot slot) noexcept;

private:
  utility::Signal<const std::string&> line_added_signal_{};
  utility::Signal<const std::optional<std::string>> channel_name_changed_signal_{};
};

}