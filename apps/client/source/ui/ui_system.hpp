#pragma once

#include "../application/client_tcp_system.hpp"
#include "../commands/client_commands_system.hpp"

#include <ebroschin/core/system.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <thread>

namespace ebroschin::chatcore::client {

class ModelSystem;
class SessionSystem;

class UiSystem final : public core::System {
public:
  explicit UiSystem(const core::SystemContext& ctx) noexcept;

  void Initialize() override;
  void Deinitialize() override;

  void Shutdown();

private:
  void ProcessThread();
  [[nodiscard]] ftxui::Element Render() const;
  [[nodiscard]] ftxui::Element RenderInputField() const;
  [[nodiscard]] ftxui::Element RenderLogDisplay() const;
  bool HandleEvent(const ftxui::Event& ftxui_event);
  void HandleInput(std::string_view input) const;

  void WriteLine(const std::string&);

  ClientCommandsSystem& commands_system_;
  ModelSystem& model_system_;
  SessionSystem& session_system_;

  std::jthread ui_thread_{};
  std::mutex mutex_{};
  std::queue<std::string> message_queue_{};
  utility::SignalSubscription line_added_subscription_{};
  utility::SignalSubscription channel_name_changed_subscription_{};

  std::deque<std::string> chat_log_view_model_{};
  std::optional<std::string> channel_name_{};
  std::string input_buffer_{};
  ftxui::ScreenInteractive screen_{ftxui::ScreenInteractive::Fullscreen()};
  ftxui::Component input_component_{};
};

}