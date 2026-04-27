#pragma once

namespace ebroschin::core {

class SystemContext;

class System {
public:
  explicit System(const SystemContext& ctx) : ctx_{ctx} {}
  virtual ~System() = default;

  virtual void Initialize() {}
  virtual void Deinitialize() {}

 protected:
  const SystemContext& ctx_;
};

}