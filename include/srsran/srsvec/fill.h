/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

/// \brief Sets all elements of a sequence to a given value.
///
/// \tparam T      Type of the sequence container, must be span-compatible.
/// \param  x      Sequence container.
/// \param  value  Value the sequence is set to. Must be compatible with the element type of the sequence.
template <typename T>
void fill(T&& x, detail::value_type_of_t<T> value)
{
  static_assert(is_span_compatible<T>::value, "Template type is not compatible with a span");
  std::fill(x.begin(), x.end(), value);
}

} // namespace srsvec
} // namespace srsran
