#include <claw/core/application.h>

namespace claw::core {

namespace {
  //only used by this translation unit
  Application* instance_ = nullptr;
}

Application::Application() noexcept
{
  instance_ = this;
}

void Application::Quit() noexcept {
  running_.store(false, std::memory_order_relaxed);
  running_.notify_one();
}

void Application::Terminate() {
  if (!instance_) return;
  instance_->HandleTerminate();
}

void Application::RunBlocking() {
  running_ = true;

  std::set_terminate(Terminate);
  Initialize();
  ctx_.Initialize();

  running_.wait(true, std::memory_order_relaxed);

  ctx_.Deinitialize();
}

void Application::RunSimulation() {
  running_ = true;

  std::set_terminate(Terminate);
  Initialize();
  ctx_.Initialize();

  /* TODO
      consider implementing a registry for specific handlers
      the specific application can register its own frame methods
      without forcing BeginFrame and EndFrame semantics to applications
      that dont use them, or might even need more than that
   */
  while (running_) {
    ctx_.BeginFrame();
    ctx_.Update();
    ctx_.EndFrame();
  }

  ctx_.Deinitialize();
}

}
