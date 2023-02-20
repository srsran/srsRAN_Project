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

#include "srsgnb/srsvec/detail/traits.h"
#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

template <typename T, typename U>
void copy(T&& dst, const U& src)
{
  static_assert(detail::is_span_compatible<T>::value, "Template type is not compatible with a span");
  static_assert(detail::is_span_compatible<U>::value, "Template type is not compatible with a span");
  srsgnb_srsvec_assert_size(dst, src);

  std::copy(src.begin(), src.end(), dst.begin());
}

} // namespace srsvec
} // namespace srsgnb
