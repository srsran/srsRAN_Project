/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_COMPARE_H
#define SRSGNB_SRSVEC_COMPARE_H

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

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_COMPARE_H
