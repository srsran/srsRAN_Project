/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_COMPARE_H_
#define SRSGNB_SRSVEC_COMPARE_H_

#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

template <typename T1, typename T2>
bool equal(const T1& s1, const T2& s2)
{
  srsgnb_srsvec_assert_size(s1, s2);

  return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end());
}

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_COMPARE_H_
