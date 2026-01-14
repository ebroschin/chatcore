#pragma once

#include <claw/core/system.h>
#include <string>

namespace claw::prototyping {

class PrototypingSystem final : public core::System {
public:
  explicit PrototypingSystem(const core::SystemContext& ctx)
    : System(ctx)
  {}

};

}