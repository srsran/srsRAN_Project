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

#ifndef SRSGNB_SRSVEC_TYPES_H_
#define SRSGNB_SRSVEC_TYPES_H_

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include <cstdint>

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

#define srsgnb_srsvec_assert_size(X, Y) assert(X.size() == Y.size())

#endif // SRSGNB_SRSVEC_TYPES_H_
