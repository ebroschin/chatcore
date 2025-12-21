#pragma once

#include <claw/core/system.h>
#include <string>

namespace claw::prototyping {

class PrototypingSystem final : public core::System {
public:
  explicit PrototypingSystem(const core::SystemContext& ctx, const std::string& argument)
    : System(ctx), argument_{argument}
  {}

  const std::string& Argument() { return argument_; }

private:
  std::string argument_;
};

}