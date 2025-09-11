/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
/// Bitwise binary operations.

#pragma once

#include "srsran/srsvec/type_traits.h"
#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

/// \brief Binary XOR.
///
/// Applies the bitwise XOR operator to corresponding elements of the input sequences.
/// \tparam T  Type of the output sequence (must be compatible with a span of integers).
/// \tparam U  Type of the first input sequence (must be compatible with a span of integers).
/// \tparam V  Type of the second input sequence (must be compatible with a span of integers).
///
/// \param[out] z  Output sequence.
/// \param[in]  x  First input sequence.
/// \param[in]  y  Second input sequence.
/// \warning An assertion is triggered if input and output sequences have different sizes.
template <typename T, typename U, typename V>
void binary_xor(T&& z, const U& x, const V& y)
{
  static_assert(is_integral_span_compatible<T>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<U>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<V>::value, "Template type is not compatible with a span of integers");
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] ^ y[i];
  }
}

/// \brief Binary AND.
///
/// Applies the bitwise AND operator to corresponding elements of the input sequences.
/// \tparam T  Type of the output sequence (must be compatible with a span of integers).
/// \tparam U  Type of the first input sequence (must be compatible with a span of integers).
/// \tparam V  Type of the second input sequence (must be compatible with a span of integers).
///
/// \param[out] z  Output sequence.
/// \param[in]  x  First input sequence.
/// \param[in]  y  Second input sequence.
/// \warning An assertion is triggered if input and output sequences have different sizes.
template <typename T, typename U, typename V>
void binary_and(T&& z, const U& x, const V& y)
{
  static_assert(is_integral_span_compatible<T>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<U>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<V>::value, "Template type is not compatible with a span of integers");
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] & y[i];
  }
}

/// \brief Binary OR.
///
/// Applies the bitwise OR operator to corresponding elements of the input sequences.
/// \tparam T  Type of the output sequence (must be compatible with a span of integers).
/// \tparam U  Type of the first input sequence (must be compatible with a span of integers).
/// \tparam V  Type of the second input sequence (must be compatible with a span of integers).
///
/// \param[out] z  Output sequence.
/// \param[in]  x  First input sequence.
/// \param[in]  y  Second input sequence.
/// \warning An assertion is triggered if input and output sequences have different sizes.
template <typename T, typename U, typename V>
void binary_or(T&& z, const U& x, const V& y)
{
  static_assert(is_integral_span_compatible<T>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<U>::value, "Template type is not compatible with a span of integers");
  static_assert(is_integral_span_compatible<V>::value, "Template type is not compatible with a span of integers");
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] | y[i];
  }
}

} // namespace srsvec
} // namespace srsran
