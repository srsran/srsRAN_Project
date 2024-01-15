/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/// \file
/// \brief SIMD support for LDPC.
///
/// Defines type aliases for SIMD implementations of LDPC blocks.

#pragma once

#if defined(__x86_64__)
#include <immintrin.h>
#endif // defined(__x86_64__)

#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif // defined(HAVE_NEON)

namespace srsran {
namespace detail {

#if defined(__x86_64__)
using simd128_type = struct {
  // Just for compatibility with the ARM implementation, not used in x86.
};
using simd256_type = __m256i;
using simd512_type = __m512i;
#endif // defined(__x86_64__)

#if defined(__ARM_NEON)
using simd128_type = int8x16_t;
using simd256_type = struct {
  // Just for compatibility with the x86 implementation, not used in ARM.
};
using simd512_type = struct {
  // Just for compatibility with the x86 implementation, not used in ARM.
};
#endif // defined(HAVE_NEON)

struct simd128_wrapper {
  using simdType = simd128_type;
};

struct simd256_wrapper {
  using simdType = simd256_type;
};

struct simd512_wrapper {
  using simdType = simd512_type;
};
} // namespace detail
} // namespace srsran