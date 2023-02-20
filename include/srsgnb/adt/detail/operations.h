
#pragma once

namespace srsran {

namespace detail {

// Do nothing operation functor.
struct noop_operation {
  template <typename T>
  void operator()(T&& t) const
  {
    // do nothing
  }
};

} // namespace detail

} // namespace srsran
