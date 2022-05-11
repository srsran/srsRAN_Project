/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_COPY_H_
#define SRSGNB_SRSVEC_COPY_H_

#include "srsgnb/srsvec/types.h"

namespace srsgnb {
namespace srsvec {

template <typename T>
void copy(span<T> dst, span<T> src)
{
  srsgnb_srsvec_assert_size(dst, src);

  std::copy(src.begin(), src.end(), dst.begin());
}

template <typename T>
void copy(span<T> dst, span<const T> src)
{
  srsgnb_srsvec_assert_size(dst, src);

  std::copy(src.begin(), src.end(), dst.begin());
}

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_COPY_H_
