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

#ifndef SRSGNB_SUPPORT_MATH_UTILS_H_
#define SRSGNB_SUPPORT_MATH_UTILS_H_

namespace srsgnb {

inline unsigned divide_ceil(unsigned num, unsigned den)
{
  assert(den != 0);
  return (num + (den - 1)) / den;
}
} // namespace srsgnb

#endif // SRSGNB_SUPPORT_MATH_UTILS_H_
