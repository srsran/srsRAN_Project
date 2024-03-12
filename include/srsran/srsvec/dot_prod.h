/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/// \file
/// \brief Dot product declarations.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/srsvec/detail/traits.h"
#include "srsran/srsvec/types.h"
#include "srsran/support/srsran_assert.h"
#include <numeric>

namespace srsran {
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
/// \return The dot product between the two spans plus \c init, i.e. \f$ x \cdot y + \textup{init} = \sum_i x_i y_i +
/// \textup{init}\f$.
/// \remark The two input spans must have the same length.
template <typename T, typename U, typename V>
inline V dot_prod(const T& x, const U& y, V init)
{
  static_assert(detail::is_arithmetic_span_compatible<T>::value,
                "Template type is not compatible with a span of arithmetics");
  static_assert(detail::is_arithmetic_span_compatible<U>::value,
                "Template type is not compatible with a span of arithmetics");
  srsran_srsvec_assert_size(x, y);
  return std::inner_product(x.begin(), x.end(), y.begin(), init);
}

/// \brief Dot product of two complex spans.
///
/// Computes the dot product (a.k.a. inner product or scalar product) of the two complex sequences. The sequences are
/// represented by the input spans \c x and \c y, the second sequence \c y is conjugated.
///
/// The implementation is equivalent to:
/// \code
/// cf_t dot_prod(span<const cf_t> x, span<const cf_t> y) {
///   return std::inner_product(x.begin(), x.end(), y.begin(), cf_t(0.0F), std::plus<>(), [](cf_t a, cf_t b) { return a
///   * std::conj(b); });
/// }
/// \endcode
///
/// \param[in] x      First span.
/// \param[in] y      Second span.
/// \return The dot product between the two spans, i.e. \f$ x \cdot \conj{y}= \sum_i x_i \conj{y}_i \f$.
/// \remark The two input spans must have the same length.
cf_t dot_prod(span<const cf_t> x, span<const cf_t> y);

/// \brief Estimates the average power of a complex span - linear scale.
///
/// The average power of a span is defined as its squared Euclidean norm divided by the number of its elements, i.e.
/// <tt>dot_prod(x, x) / x.size()</tt>.
inline float average_power(span<const cf_t> x)
{
  if (x.empty()) {
    return 0.0F;
  }

  return std::real(srsvec::dot_prod(x, x)) / static_cast<float>(x.size());
}

} // namespace srsvec
} // namespace srsran
