/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/srsvec/type_traits.h"
#include "srsran/srsvec/types.h"

namespace srsran {
namespace srsvec {

/// \brief Sets all elements of a sequence to zero.
///
/// \tparam T Type of the sequence container, must be span-compatible.
/// \param  x Sequence container.
template <typename T>
void zero(T&& x)
{
  static_assert(is_span_compatible<T>::value, "Template type is not compatible with a span.");
  std::fill(x.begin(), x.end(), 0);
}

} // namespace srsvec
} // namespace srsran
