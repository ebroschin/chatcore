#pragma once

#include <memory>
#include <claw/core/system_context.h>
#include <atomic>

namespace claw::core {

/**
 * @brief Override with your specific implementation.
 * Call Application::Run() to start the life cycle of the program.
 */
class Application {
public:
  Application() noexcept;
  virtual ~Application() = default;

  Application(const Application& other) = delete;
  void operator=(const Application& other) = delete;
  Application(Application&& other) = delete;
  void operator=(Application&& other) = delete;

  /**
 * @brief Starts the program in blocking mode:
 * - Calls Application::Initialize()
 * - Calls ctx_->Initialize()
 * - Performs cleanup via ctx_->Deinitialize() when Quit() is called
 */
  void RunBlocking();

  void Quit() noexcept;

  [[nodiscard]] bool IsRunning() const noexcept
  { return running_.load(std::memory_order::relaxed); }

protected:
  /**
   * @brief Override to register application systems via `ctx_->Register()`.
   * Runs immediately before SystemContext::Initialize().
   */
  virtual void Initialize() = 0;

  /**
   * @brief Executes when the program is terminated.
   */
  virtual void HandleTerminate() {}

  SystemContext ctx_;
  std::atomic<bool> running_{false};

private:
  static void Terminate();

};

}