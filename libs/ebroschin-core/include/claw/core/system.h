#pragma once

namespace claw::core {

class SystemContext;

class System {
public:
  explicit System(const SystemContext& ctx) : ctx_{ctx} {}
  virtual ~System() = default;

  virtual void Initialize() {}
  virtual void Deinitialize() {}

  //TODO remove these from system, create a super class with simulation semantics instead
  virtual void BeginFrame() {}
  virtual void Update() {}
  virtual void EndFrame() {}

 protected:
  const SystemContext& ctx_;
};

}