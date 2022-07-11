
#pragma once

namespace srsgnb {

namespace detail {

// Do nothing operator
struct noop_operator {
  template <typename T>
  void operator()(T&& t) const
  {
    // do nothing
  }
};

} // namespace detail

} // namespace srsgnb
