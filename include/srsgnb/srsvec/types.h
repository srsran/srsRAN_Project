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

#ifndef SRSGNB_SRSVEC_TYPES_H_
#define SRSGNB_SRSVEC_TYPES_H_

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include <stdint.h>

/**
 * The supported vector data types are:
 * - srsgnb::span<cf_t>: For complex float vectors
 * - srsgnb::span<float>: For float vectors
 * - srsgnb::span<int16_t>: For signed 16 bit integer vectors
 * - srsgnb::span<int8_t>: For signed 8 bit integer vectors
 * - srsgnb::span<uint8_t>: For unsigned 8 bit integer vectors
 *
 * To make the span constant follow srsgnb::span<const T>
 *
 */

#define srsgnb_srsvec_assert_type(T)                                                                                   \
  static_assert(std::is_same<T, cf_t>::value or std::is_same<T, float>::value or std::is_same<T, int16_t>::value or    \
                    std::is_same<T, int8_t>::value or std::is_same<T, uint8_t>::value,                                 \
                "Function requires cf_t, float, int16_t, int8_t, or uint8_t types")

#define srsgnb_srsvec_assert_size(X, Y) assert(X.size() == Y.size())

#endif // SRSGNB_SRSVEC_TYPES_H_
