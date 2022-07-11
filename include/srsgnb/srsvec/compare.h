/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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

template <typename T1, typename T2>
bool equal(const T1& s1, const T2& s2)
{
  static_assert(detail::is_span_compatible<T1>::value, "Template type is not compatible with a span");
  static_assert(detail::is_span_compatible<T2>::value, "Template type is not compatible with a span");
  srsgnb_srsvec_assert_size(s1, s2);

  return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end());
}

/// \brief Finds the maximum absolute value in a complex span.
///
/// In case two elements have the same absolute value, the one with lowest index is selected.
///
/// The implementation is equivalent to:
/// \code
///  std::pair<unsigned, float> max_abs_element(span<const cf_t> x)
///    const cf_t* it = std::max_element(x.begin(), x.end(), [](cf_t a, cf_t b) { return (abs_sq(a) < abs_sq(b)); });
///    return {static_cast<unsigned>(it - x.begin()), abs_sq(*it)};
///  }
/// \endcode
///
/// \param[in] x Input samples.
/// \return A pair comprising the index and the squared modulo of the maximum element.
std::pair<unsigned, float> max_abs_element(span<const cf_t> x);

} // namespace srsvec
} // namespace srsgnb
