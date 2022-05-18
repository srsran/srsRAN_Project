/// \file
/// \brief Dot product declarations.

#ifndef SRSGNB_SRSVEC_DOT_PROD_H
#define SRSGNB_SRSVEC_DOT_PROD_H

#include "srsgnb/adt/span.h"
#include "srsgnb/srsvec/detail/traits.h"
#include "srsgnb/srsvec/types.h"
#include "srsgnb/support/srsran_assert.h"

#include <numeric>

namespace srsgnb {
namespace srsvec {

/// \brief Dot product of two spans.
///
/// Computes the dot product (a.k.a. inner product or scalar product) of the two sequences represented by the input
/// spans, adding an initial offset.
/// \tparam T         A span of an arithmetic type.
/// \tparam U         A span of an arithmetic type.
/// \tparam V         Output type (must be compatible with the product of object of type \c T and \c U).
/// \param[in] x      First span.
/// \param[in] y      Second span.
/// \param[in] init   Initialization value.
/// \return The dot product between the two spans plus \c init, i.e. \f$ x \cdot y + \mathrm{init} = \sum_i x_i y_i +
/// \mathrm{init}\f$.
/// \remark The two input spans must have the same length.
template <typename T, typename U, typename V>
inline V dot_prod(const T& x, const U& y, V init)
{
  static_assert(detail::is_arithmetic_span_compatible<T>::value,
                "Template type is not compatible with a span of arithmetics");
  static_assert(detail::is_arithmetic_span_compatible<U>::value,
                "Template type is not compatible with a span of arithmetics");
  srsgnb_srsvec_assert_size(x, y);
  return std::inner_product(x.begin(), x.end(), y.begin(), init);
}

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_DOT_PROD_H
