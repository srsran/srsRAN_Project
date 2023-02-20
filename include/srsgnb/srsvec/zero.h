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

namespace srsran {
namespace srsvec {

template <typename T>
void zero(T&& x)
{
  static_assert(detail::is_span_compatible<T>::value, "Template type is not compatible with a span");
  std::fill(x.begin(), x.end(), 0);
}

} // namespace srsvec
} // namespace srsran
