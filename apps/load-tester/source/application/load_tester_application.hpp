#pragma once

#include <ebroschin/core/application.hpp>

#include "load_tester_arguments.hpp"

namespace ebroschin::chatcore::tester {

class LoadTesterApplication final : public core::Application {
public:
  explicit LoadTesterApplication(LoadTesterArguments arguments) noexcept;

  [[nodiscard]] const LoadTesterArguments& GetArguments() const noexcept
  { return arguments_; }

protected:
  void Initialize() override;
  void HandleTerminate() override;

private:
  LoadTesterArguments arguments_;
};

}