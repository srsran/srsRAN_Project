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

#include <immintrin.h>

namespace srsgnb {

namespace avx2 {

inline __m256 abs(__m256 x)
{
  const __m256 mask = _mm256_castsi256_ps(_mm256_set1_epi32(0x7fffffff));
  return _mm256_and_ps(x, mask);
}

inline __m256 copysign(__m256 x, __m256 y)
{
  __m256 sign_y = _mm256_and_ps(y, _mm256_set1_ps(-0.0));
  __m256 abs_x  = abs(x);
  return _mm256_or_ps(abs_x, sign_y);
}

inline __m256 clip(__m256 x, __m256 range_ceil, __m256 range_floor)
{
  x = _mm256_blendv_ps(x, range_ceil, _mm256_cmp_ps(x, range_ceil, _CMP_GT_OS));
  x = _mm256_blendv_ps(x, range_floor, _mm256_cmp_ps(x, range_floor, _CMP_LT_OS));
  return x;
}

inline __m256i quantize(__m256 value_0, __m256 value_1, __m256 value_2, __m256 value_3, float range_limit)
{
  // Clipping.
  __m256 RANGE_CEIL  = _mm256_set1_ps(range_limit);
  __m256 RANGE_FLOOR = _mm256_set1_ps(-range_limit);
  value_0            = clip(value_0, RANGE_CEIL, RANGE_FLOOR);
  value_1            = clip(value_1, RANGE_CEIL, RANGE_FLOOR);
  value_2            = clip(value_2, RANGE_CEIL, RANGE_FLOOR);
  value_3            = clip(value_3, RANGE_CEIL, RANGE_FLOOR);

  // Scale and round to the nearest integer.
  __m256 SCALE = _mm256_set1_ps(static_cast<float>(log_likelihood_ratio::max().to_int()) / range_limit);
  value_0      = _mm256_round_ps(_mm256_mul_ps(value_0, SCALE), _MM_FROUND_NINT);
  value_1      = _mm256_round_ps(_mm256_mul_ps(value_1, SCALE), _MM_FROUND_NINT);
  value_2      = _mm256_round_ps(_mm256_mul_ps(value_2, SCALE), _MM_FROUND_NINT);
  value_3      = _mm256_round_ps(_mm256_mul_ps(value_3, SCALE), _MM_FROUND_NINT);

  // Convert to 32 bit.
  __m256i llr_i32_0 = _mm256_cvtps_epi32(value_0);
  __m256i llr_i32_1 = _mm256_cvtps_epi32(value_1);
  __m256i llr_i32_2 = _mm256_cvtps_epi32(value_2);
  __m256i llr_i32_3 = _mm256_cvtps_epi32(value_3);

  // Re-collocate SSE registers.
  __m256i llr_i32_0_ = _mm256_permute2f128_si256(llr_i32_0, llr_i32_1, 0x20);
  __m256i llr_i32_1_ = _mm256_permute2f128_si256(llr_i32_0, llr_i32_1, 0x31);
  __m256i llr_i32_2_ = _mm256_permute2f128_si256(llr_i32_2, llr_i32_3, 0x20);
  __m256i llr_i32_3_ = _mm256_permute2f128_si256(llr_i32_2, llr_i32_3, 0x31);

  // Conversion to 16 bit.
  __m256i llr_i16_0 = _mm256_packs_epi32(llr_i32_0_, llr_i32_1_);
  __m256i llr_i16_1 = _mm256_packs_epi32(llr_i32_2_, llr_i32_3_);

  // Re-collocate SSE registers.
  __m256i llr_i16_0_ = _mm256_permute2f128_si256(llr_i16_0, llr_i16_1, 0x20);
  __m256i llr_i16_1_ = _mm256_permute2f128_si256(llr_i16_0, llr_i16_1, 0x31);

  // Conversion to 8 bit.
  return _mm256_packs_epi16(llr_i16_0_, llr_i16_1_);
}

} // namespace avx2
} // namespace srsgnb