
#pragma once

namespace srsran {

/// Do nothing operation functor.
struct noop_operation {
  template <typename T>
  void operator()(T&& t) const
  {
    // Do nothing.
  }
};

} // namespace srsran
