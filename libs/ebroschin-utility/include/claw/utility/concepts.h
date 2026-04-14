#pragma once

namespace claw::utility {

template <typename Derived, typename Base>
concept VirtuallyDerivedFrom = std::derived_from<Derived, Base> && !requires(Base* b) {
  static_cast<Derived*>(b);
};

}

