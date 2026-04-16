#include "ui_system.h"

#include "ftxui_logger.hpp"

#include <ebroschin/core/application.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <mutex>
#include <ranges>

#include "../model/model_system.h"
#include "../session/session_system.h"
#include "ebroschin/logging/log.hpp"

using namespace std::chrono_literals;

namespace ebroschin::chatcore::client {

UiSystem::UiSystem(const core::SystemContext& ctx, core::Application& app):
  System(ctx),
  app_(app),
  commands_system_(ctx.Require<ClientCommandsSystem>()),
  model_system_(ctx.Require<ModelSystem>()),
  user_system_(ctx.Require<SessionSystem>())
{ }

void UiSystem::Initialize() {
  ebroschin::logging::Log::SetLogger<FtxuiLogger>(model_system_, *this);
  WriteLine("Welcome to ChatCore | Developed by Elias Broschin");
  WriteLine("-------------------------------------------------");
  WriteLine("Enter /help for more details");

  ui_thread_ = std::jthread{[this] { ProcessThread(); }};
  line_added_subscription_ = model_system_.OnLineAdded([this](const std::string& line) {
    {
      std::unique_lock lock(mutex_);
      message_queue_.emplace(line);
    }

    screen_.PostEvent(ftxui::Event::Custom);
  });

  channel_name_changed_subscription_ = model_system_.OnChannelNameChanged([this](std::optional<std::string> channel_name) {
    channel_name_ = std::move(channel_name);
    screen_.PostEvent(ftxui::Event::Custom);
  });
}

void UiSystem::Deinitialize() {
  line_added_subscription_.Unsubscribe();
  Shutdown();
  ui_thread_ = {};
}

void UiSystem::Shutdown() {
  screen_.Exit();
}

void UiSystem::ProcessThread() {
  input_component_ = ftxui::Input(&input_buffer_, "Type a message or /help");
  const auto ui = ftxui::Renderer(input_component_, [this] { return Render(); });
  const auto app = ftxui::CatchEvent(ui, [this](const ftxui::Event& event) { return HandleEvent(event); });

  input_component_->TakeFocus();
  screen_.Loop(app);
}

void UiSystem::WriteLine(const std::string& line) {
  chat_log_view_model_.emplace_back(line);

  if (chat_log_view_model_.size() < 100) return;
  chat_log_view_model_.pop_front();
}

ftxui::Element UiSystem::Render() const {
  return ftxui::vbox({
    RenderLogDisplay(),
    ftxui::separator(),
    RenderInputFíeld()
  }) | ftxui::flex;
}

ftxui::Element UiSystem::RenderLogDisplay() const {
  ftxui::Elements text_lines;
  text_lines.reserve(chat_log_view_model_.size());

  for (const auto& line : chat_log_view_model_) {
    auto text_element = ftxui::paragraph(line) | ftxui::color(ftxui::Color::Green);
    text_lines.push_back(std::move(text_element));
  }

  return vbox(std::move(text_lines))
      | ftxui::focusPositionRelative(0.0f, 1.0f)
      | ftxui::yframe
      | ftxui::vscroll_indicator
      | ftxui::flex;
}

ftxui::Element UiSystem::RenderInputFíeld() const {
  const auto prompt = channel_name_.has_value()?
    "chat::" + *channel_name_ + "> ":
    "chat> ";

  return ftxui::hbox({
      ftxui::text(prompt),
      input_component_->Render() | ftxui::flex
  });
}

bool UiSystem::HandleEvent(const ftxui::Event& e) {
  if (e == ftxui::Event::Custom) {
    std::queue<std::string> buffer;
    {
      std::lock_guard lock(mutex_);
      if (message_queue_.empty()) return true;
      buffer.swap(message_queue_);
    }

    while (!buffer.empty()) {
      WriteLine(std::move(buffer.front()));
      buffer.pop();
    }

    return true;
  }

  if (e == ftxui::Event::Return) {
    if (!input_buffer_.empty()) {
      HandleInput(input_buffer_);
      input_buffer_.clear();
    }

    return true;
  }

  if (e == ftxui::Event::Escape) {
    app_.Quit();
    return true;
  }

  return false;
}

void UiSystem::HandleInput(std::string_view input) const {
  if (input.empty()) return;

  if (input[0] != '/') {
    user_system_.Send(std::string(input));
    return;
  }

  std::vector<std::string_view> split;
  for (const auto& argument : input | std::views::split(' ')) {
    split.emplace_back(argument);
  }

  const auto token = std::string(split[0] | std::views::drop(1));
  auto success = commands_system_.Execute(token, split | std::views::drop(1));
  if (success) return;

  ebroschin::logging::Log::Error() << "unknown command: " << token;
}

}