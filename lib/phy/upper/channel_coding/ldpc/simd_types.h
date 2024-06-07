/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#endif // defined(__ARM_NEON)

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
#endif // defined(__ARM_NEON)

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