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

#pragma once

#include "srsran/srsvec/detail/traits.h"
#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

template <typename T, typename U, typename V>
void binary_xor(const T& x, const U& y, V&& z)
{
  static_assert(detail::is_integral_span_compatible<T>::value,
                "Template type is not compatible with a span of integers");
  static_assert(detail::is_integral_span_compatible<U>::value,
                "Template type is not compatible with a span of integers");
  static_assert(detail::is_integral_span_compatible<V>::value,
                "Template type is not compatible with a span of integers");
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] ^ y[i];
  }
}

template <typename T, typename U, typename V>
void binary_and(const T& x, const U& y, V&& z)
{
  static_assert(detail::is_integral_span_compatible<T>::value,
                "Template type is not compatible with a span of integers");
  static_assert(detail::is_integral_span_compatible<U>::value,
                "Template type is not compatible with a span of integers");
  static_assert(detail::is_integral_span_compatible<V>::value,
                "Template type is not compatible with a span of integers");
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] & y[i];
  }
}

template <typename T, typename U, typename V>
void binary_or(const T& x, const U& y, V&& z)
{
  static_assert(detail::is_integral_span_compatible<T>::value,
                "Template type is not compatible with a span of integers");
  static_assert(detail::is_integral_span_compatible<U>::value,
                "Template type is not compatible with a span of integers");
  static_assert(detail::is_integral_span_compatible<V>::value,
                "Template type is not compatible with a span of integers");
  srsran_srsvec_assert_size(x, y);
  srsran_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] | y[i];
  }
}

} // namespace srsvec
} // namespace srsran
