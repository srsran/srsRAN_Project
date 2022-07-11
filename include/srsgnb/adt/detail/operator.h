
#ifndef SRSGNB_ADT_DETAIL_OPERATOR_H
#define SRSGNB_ADT_DETAIL_OPERATOR_H

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

#endif // SRSGNB_ADT_DETAIL_OPERATOR_H
