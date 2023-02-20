/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

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
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

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
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] | y[i];
  }
}

} // namespace srsvec
} // namespace srsran
