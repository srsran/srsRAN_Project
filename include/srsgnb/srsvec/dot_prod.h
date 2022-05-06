/// \file
/// \brief Dot product declarations.

#ifndef SRSGNB_SRSVEC_DOT_PROD_H
#define SRSGNB_SRSVEC_DOT_PROD_H

#include "srsgnb/adt/span.h"
#include "srsgnb/support/srsran_assert.h"

#include <numeric>

namespace srsgnb {
namespace srsvec {

/// \brief Dot product of two spans.
///
/// Computes the dot product (a.k.a. inner product or scalar product) of the two sequences represented by the input
/// spans, adding an initial offset.
/// \tparam T         Type of the span elements.
/// \tparam U         Type of the output and of the initialization value.
/// \param[in] x      First span.
/// \param[in] y      Second span.
/// \param[in] init   Initialization value.
/// \return The dot product between the two spans plus \c init, i.e. \f$ x \cdot y + \mathrm{init} = \sum_i x_i y_i +
/// \mathrm{init}\f$.
/// \remark The two input spans must have the same length.
template <typename T, typename U>
inline U dot_prod(span<const T> x, span<const T> y, U init)
{
  srsran_assert(x.size() == y.size(), "Inputs must have the same length.");
  return std::inner_product(x.begin(), x.end(), y.begin(), init);
}

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_DOT_PROD_H
