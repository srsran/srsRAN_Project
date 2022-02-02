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

#ifndef SRSGNB_SRSVEC_BINARY_H_
#define SRSGNB_SRSVEC_BINARY_H_

#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

template <typename T>
void binary_xor(span<T> x, span<T> y, span<T> z)
{
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] ^ y[i];
  }
}

template <typename T>
void binary_and(span<T> x, span<T> y, span<T> z)
{
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] & y[i];
  }
}

template <typename T>
void binary_or(span<T> x, span<T> y, span<T> z)
{
  srsgnb_srsvec_assert_size(x, y);
  srsgnb_srsvec_assert_size(x, z);

  for (std::size_t i = 0, len = x.size(); i != len; ++i) {
    z[i] = x[i] | y[i];
  }
}

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_BINARY_H_
