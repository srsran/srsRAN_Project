/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
/// \brief Convolution declarations.

#pragma once

#include "srsran/srsvec/detail/traits.h"
#include "srsran/srsvec/types.h"
#include <numeric>

namespace srsran {
namespace srsvec {

namespace detail {

/// Check if two span-compatibles are the different: always true if they are of different type.
template <typename T, typename U>
bool check_different(const T& /* na */, const U& /* na */)
{
  return true;
}

/// Check if two span-compatibles are the different: if they are of the same type, check the data pointers.
template <typename T>
bool check_different(const T& in1, const T& in2)
{
  return (in1.data() != in2.data());
}

} // namespace detail

/// \brief Convolution between two sequences.
///
/// Computes the convolution between two sequences \f$x\f$ and \f$y\f$, namely \f$z[n] = \sum_{i} x[i] y[n-i]\f$ and
/// returns its central part, of the \e same size as the first input sequence.
/// \tparam V   Output type (must be compatible with a span of arithmetic/complex values).
/// \tparam T   Type of the first input sequence (must be compatible with a span of arithmetic/complex values).
/// \tparam U   Type of the second input sequence (must be compatible with a span of arithmetic/complex values).
/// \param[out] out  Output sequence - convolution between the input sequences.
/// \param[in]  x_v  First input sequence.
/// \param[in]  y_v  Second input sequence.
/// \remark An assertion is raised if the length of the second sequence is larger than the length of the first one.
/// \remark This implementation of the convolution is not based on FFT and should only be used when the second input
/// sequence is "very" short.
template <typename V, typename T, typename U>
void convolution_same(V&& out, const T& x_v, const U& y_v)
{
  static_assert((detail::is_arithmetic_span_compatible<T>::value || detail::is_complex_span_compatible<T>::value),
                "Template type is not compatible with a span of arithmetics or complex floats.");
  static_assert((detail::is_arithmetic_span_compatible<U>::value || detail::is_complex_span_compatible<U>::value),
                "Template type is not compatible with a span of arithmetics or complex floats.");
  static_assert((detail::is_arithmetic_span_compatible<V>::value || detail::is_complex_span_compatible<V>::value),
                "Template type is not compatible with a span of arithmetics or complex floats.");
  srsran_srsvec_assert_size(out, x_v);

  unsigned x_size = x_v.size();
  unsigned y_size = y_v.size();
  srsran_assert((y_size <= x_size),
                "The current implementation of the convolution is only defined when the second input is not longer "
                "than the first one.");

  using Type_y   = typename U::value_type;
  using Type_x   = typename T::value_type;
  using Type_out = typename std::remove_reference<V>::type::value_type;

  srsran_assert(detail::check_different(out, x_v), "Cannot override input with output.");

  span<const Type_x> x(x_v);
  span<const Type_y> y(y_v);
  Type_out           init = 0;

  // At the beginning, x and y do not fully overlap.
  unsigned skip  = y_size / 2;
  unsigned i_out = 0;
  for (unsigned n_els = skip + 1; n_els != y_size; ++n_els) {
    span<const Type_y> y_local = y.first(n_els);
    span<const Type_x> x_local = x.first(n_els);
    // Note that y_local is reversed.
    out[i_out++] = std::inner_product(x_local.begin(), x_local.end(), y_local.rbegin(), init);
  }

  // In the central part, y fully overlaps with x.
  unsigned start = i_out;
  for (unsigned i_last = x_size + start - y_size + 1; i_out != i_last; ++i_out) {
    span<const Type_x> x_local = x.subspan(i_out - start, y_size);
    // Note that y_local is reversed.
    out[i_out] = std::inner_product(x_local.begin(), x_local.end(), y.rbegin(), init);
  }

  // For the final part, we again take into account the partial overlap.
  for (unsigned n_els = x_size - i_out + start; n_els != start; --n_els) {
    span<const Type_y> y_local = y.last(n_els);
    span<const Type_x> x_local = x.last(n_els);
    // Note that y_local is reversed.
    out[i_out++] = std::inner_product(x_local.begin(), x_local.end(), y_local.rbegin(), init);
  }
}

} // namespace srsvec
} // namespace srsran