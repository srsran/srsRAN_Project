/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/complex.h"

// Enables intel intrinsics, it includes AVX, AVX2, FMA, AVX512.
#ifdef __SSE4_1__

// gcc-12 gives a likely false alarm when including AVX512 intrinsics. Disable maybe-uninitialized diagnostics.
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif // __clang__

#include <immintrin.h>

#ifndef __clang__
#pragma GCC diagnostic pop
#endif // __clang__
#endif // __SSE4_1__

#ifdef __ARM_NEON
#include <arm_neon.h>
#endif // __ARM_NEON

namespace srsran {

inline bool is_simd_addr_aligned(const void* addr, uintptr_t mask)
{
  uintptr_t addr_i = reinterpret_cast<uintptr_t>(addr);
  return (addr_i & mask) == 0;
}

///
/// SIMD Vector bit alignment.
///

#ifdef __AVX512F__
constexpr unsigned SIMD_BYTE_ALIGN = 64;
inline bool        SIMD_IS_ALIGNED(const void* ptr)
{
  return is_simd_addr_aligned(ptr, 0x3f);
}
#else /* __AVX512F__ */
#ifdef __AVX__
constexpr unsigned SIMD_BYTE_ALIGN = 32;
inline bool        SIMD_IS_ALIGNED(const void* ptr)
{
  return is_simd_addr_aligned(ptr, 0x1f);
}
#else /* __AVX__ */
#ifdef __SSE4_1__
constexpr unsigned SIMD_BYTE_ALIGN = 16;
inline bool        SIMD_IS_ALIGNED(const void* ptr)
{
  return is_simd_addr_aligned(ptr, 0x0f);
}
#else  /* __SSE4_1__ */
constexpr unsigned SIMD_BYTE_ALIGN = 16;
inline bool        SIMD_IS_ALIGNED(const void* ptr)
{
  return true;
}
#endif /* __SSE4_1__ */
#endif /* __AVX__ */
#endif /* __AVX512F__ */

///
/// Memory Sizes for Single Floating Point and fixed point.
///

#ifdef __AVX512F__

#define SRSRAN_SIMD_F_SIZE 16
#define SRSRAN_SIMD_CF_SIZE 16

#define SRSRAN_SIMD_I_SIZE 16

#define SRSRAN_SIMD_B_SIZE 64
#define SRSRAN_SIMD_S_SIZE 32
#define SRSRAN_SIMD_C16_SIZE 0

#else
#ifdef __AVX2__

#define SRSRAN_SIMD_F_SIZE 8
#define SRSRAN_SIMD_CF_SIZE 8

#define SRSRAN_SIMD_I_SIZE 8

#define SRSRAN_SIMD_B_SIZE 32
#define SRSRAN_SIMD_S_SIZE 16
#define SRSRAN_SIMD_C16_SIZE 16

#else /* __AVX2__ */
#ifdef __SSE4_1__

#define SRSRAN_SIMD_F_SIZE 4
#define SRSRAN_SIMD_CF_SIZE 4

#define SRSRAN_SIMD_I_SIZE 4

#define SRSRAN_SIMD_B_SIZE 16
#define SRSRAN_SIMD_S_SIZE 8
#define SRSRAN_SIMD_C16_SIZE 8

#else /* __SSE4_1__ */
#ifdef __ARM_NEON

#define SRSRAN_SIMD_F_SIZE 4
#define SRSRAN_SIMD_CF_SIZE 4

#define SRSRAN_SIMD_I_SIZE 4
#define SRSRAN_SIMD_B_SIZE 16
#define SRSRAN_SIMD_S_SIZE 8
#define SRSRAN_SIMD_C16_SIZE 8

#else /* __ARM_NEON */
#define SRSRAN_SIMD_F_SIZE 0
#define SRSRAN_SIMD_CF_SIZE 0

#define SRSRAN_SIMD_I_SIZE 0
#define SRSRAN_SIMD_B_SIZE 0
#define SRSRAN_SIMD_S_SIZE 0
#define SRSRAN_SIMD_C16_SIZE 0

#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */

#ifndef ENABLE_C16
#undef SRSRAN_SIMD_C16_SIZE
#define SRSRAN_SIMD_C16_SIZE 0
#endif /* ENABLE_C16 */

#if SRSRAN_SIMD_F_SIZE

///
/// Data types.
///

#ifdef __AVX512F__
using simd_f_t = __m512;
#else /* __AVX512F__ */
#ifdef __AVX2__
using simd_f_t = __m256;
#else /* __AVX2__ */
#ifdef __SSE4_1__
using simd_f_t = __m128;
#else /* __ARM_NEON */
#ifdef __ARM_NEON
using simd_f_t = float32x4_t;
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */

///
/// Single precision Floating point functions.
///

inline simd_f_t srsran_simd_f_load(const float* ptr)
{
#ifdef __AVX512F__
  return _mm512_load_ps(ptr);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_load_ps(ptr);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_load_ps(ptr);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vld1q_f32(ptr);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_loadu(const float* ptr)
{
#ifdef __AVX512F__
  return _mm512_loadu_ps(ptr);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_loadu_ps(ptr);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_loadu_ps(ptr);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vld1q_f32(ptr);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_f_store(float* ptr, simd_f_t simdreg)
{
#ifdef __AVX512F__
  _mm512_store_ps(ptr, simdreg);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_store_ps(ptr, simdreg);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_store_ps(ptr, simdreg);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  vst1q_f32(ptr, simdreg);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_f_storeu(float* ptr, simd_f_t simdreg)
{
#ifdef __AVX512F__
  _mm512_storeu_ps(ptr, simdreg);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_storeu_ps(ptr, simdreg);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_storeu_ps(ptr, simdreg);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  vst1q_f32(ptr, simdreg);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

// Interleaves values from two input vectors and stores the resulting vectors in memory.
inline void srsran_simd_f_storeu_interleaved(float* ptr, simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  __m512 s1 = _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      b);
  __m512 s2 = _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0a, 0x1a, 0x0b, 0x1b, 0x0c, 0x1c, 0x0d, 0x1d, 0x0e, 0x1e, 0x0f, 0x1f),
      b);
  _mm512_storeu_ps(reinterpret_cast<float*>(ptr), s1);
  _mm512_storeu_ps(reinterpret_cast<float*>(ptr + 16), s2);
#else /* __AVX512F__ */
#ifdef __AVX2__
  // Permute for AVX registers (reorders data across 128-bit registers).
  const __m256i idx   = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);
  __m256        tmp_a = _mm256_permutevar8x32_ps(a, idx);
  __m256        tmp_b = _mm256_permutevar8x32_ps(b, idx);

  __m256 out1 = _mm256_permute_ps(tmp_a, 0b11011000);
  __m256 out2 = _mm256_permute_ps(tmp_b, 0b11011000);
  _mm256_storeu_ps(reinterpret_cast<float*>(ptr), _mm256_unpacklo_ps(out1, out2));
  _mm256_storeu_ps(reinterpret_cast<float*>(ptr + 8), _mm256_unpackhi_ps(out1, out2));
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_storeu_ps(reinterpret_cast<float*>(ptr), _mm_unpacklo_ps(a, b));
  _mm_storeu_ps(reinterpret_cast<float*>(ptr + 4), _mm_unpackhi_ps(a, b));
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  float32x4x2_t ab_combined;
  ab_combined.val[0] = a;
  ab_combined.val[1] = b;
  vst2q_f32(reinterpret_cast<float*>(ptr), ab_combined);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_set1(float x)
{
#ifdef __AVX512F__
  return _mm512_set1_ps(x);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_set1_ps(x);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_set1_ps(x);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vdupq_n_f32(x);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_mul(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  return _mm512_mul_ps(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_mul_ps(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_mul_ps(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vmulq_f32(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_rcp(simd_f_t a)
{
#ifdef __AVX512F__
  return _mm512_rcp14_ps(a);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_rcp_ps(a);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_rcp_ps(a);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vmulq_f32(vrecpeq_f32(a), vrecpsq_f32(vrecpeq_f32(a), a));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_addsub(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  __m512 r = _mm512_add_ps(a, b);
  return _mm512_mask_sub_ps(r, 0b0101010101010101, a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_addsub_ps(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_addsub_ps(a, b);
#else             /* __SSE4_1__ */
#ifdef __ARM_NEON // CURRENTLY USES GENERIC IMPLEMENTATION FOR NEON
  const float* a_ptr = reinterpret_cast<const float*>(&a);
  const float* b_ptr = reinterpret_cast<const float*>(&b);
  simd_f_t     ret;
  float*       c_ptr = reinterpret_cast<float*>(&ret);
  for (int i = 0; i != 4; ++i) {
    if (i % 2 == 0) {
      c_ptr[i] = a_ptr[i] - b_ptr[i];
    } else {
      c_ptr[i] = a_ptr[i] + b_ptr[i];
    }
  }
  return ret;
#endif            /* __ARM_NEON */
#endif            /* __SSE4_1__ */
#endif            /* __AVX2__ */
#endif            /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_sub(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  return _mm512_sub_ps(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_sub_ps(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_sub_ps(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vsubq_f32(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_precise_rcp(simd_f_t x)
{
  // Initial approximation of 1/x
  simd_f_t approx = srsran_simd_f_rcp(x);

  // Refine using one Newton-Raphson iteration.
  simd_f_t mult       = srsran_simd_f_mul(x, approx);
  simd_f_t two        = srsran_simd_f_set1(2.0f);
  simd_f_t correction = srsran_simd_f_sub(two, mult);
  simd_f_t refined    = srsran_simd_f_mul(approx, correction);

  return refined;
}

inline simd_f_t srsran_simd_f_add(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  return _mm512_add_ps(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_add_ps(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_add_ps(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vaddq_f32(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_fma(simd_f_t acc, simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  return _mm512_fmadd_ps(a, b, acc);
#elif defined(__FMA__)
  return _mm256_fmadd_ps(a, b, acc);
#elif defined(__AVX2__)
  return _mm256_add_ps(_mm256_mul_ps(a, b), acc);
#elif defined(__SSE4_1__)
  return _mm_add_ps(_mm_mul_ps(a, b), acc);
#elif defined(__ARM_NEON)
  return vmlaq_f32(acc, a, b);
#endif
}

inline simd_f_t srsran_simd_f_zero()
{
#ifdef __AVX512F__
  return _mm512_setzero_ps();
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_setzero_ps();
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_setzero_ps();
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vdupq_n_f32(0);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_swap(simd_f_t a)
{
#ifdef __AVX512F__
  return _mm512_permute_ps(a, 0b10110001);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_permute_ps(a, 0b10110001);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_shuffle_ps(a, a, 0b10110001);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vcombine_f32(vrev64_f32(vget_low_f32(a)), vrev64_f32(vget_high_f32(a)));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_hadd(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  const __m512i idx1 = _mm512_setr_epi32((0b00000),
                                         (0b00010),
                                         (0b00100),
                                         (0b00110),
                                         (0b01000),
                                         (0b01010),
                                         (0b01100),
                                         (0b01110),
                                         (0b10000),
                                         (0b10010),
                                         (0b10100),
                                         (0b10110),
                                         (0b11000),
                                         (0b11010),
                                         (0b11100),
                                         (0b11110));
  const __m512i idx2 = _mm512_or_epi32(idx1, _mm512_set1_epi32(1));

  simd_f_t a1 = _mm512_permutex2var_ps(a, idx1, b);
  simd_f_t b1 = _mm512_permutex2var_ps(a, idx2, b);
  return _mm512_add_ps(a1, b1);
#else /* __AVX512F__ */
#ifdef __AVX2__
  simd_f_t a1 = _mm256_permute2f128_ps(a, b, 0b00100000);
  simd_f_t b1 = _mm256_permute2f128_ps(a, b, 0b00110001);
  return _mm256_hadd_ps(a1, b1);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_hadd_ps(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vcombine_f32(vpadd_f32(vget_low_f32(a), vget_high_f32(a)), vpadd_f32(vget_low_f32(b), vget_high_f32(b)));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_sqrt(simd_f_t a)
{
#ifdef __AVX512F__
  return _mm512_sqrt_ps(a);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_sqrt_ps(a);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_sqrt_ps(a);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  float32x4_t sqrt_reciprocal = vrsqrteq_f32(a);
  sqrt_reciprocal    = vmulq_f32(vrsqrtsq_f32(vmulq_f32(a, sqrt_reciprocal), sqrt_reciprocal), sqrt_reciprocal);
  float32x4_t result = vmulq_f32(a, sqrt_reciprocal);

  // Detect zeros in NEON 1/sqrtf for preventing NaN.
  // Zero vector.
  float32x4_t zeros = vmovq_n_f32(0);
  // Zero vector mask.
  uint32x4_t mask = vceqq_f32(a, zeros);
  // Force zero results and return.
  return vbslq_f32(mask, zeros, result);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_neg(simd_f_t a)
{
#ifdef __AVX512F__
  return _mm512_xor_ps(_mm512_set1_ps(-0.0f), a);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_xor_ps(_mm256_set1_ps(-0.0f), a);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_xor_ps(_mm_set1_ps(-0.0f), a);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vnegq_f32(a);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_neg_mask(simd_f_t a, simd_f_t mask)
{
#ifdef __AVX512F__
  return _mm512_xor_ps(mask, a);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_xor_ps(mask, a);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_xor_ps(mask, a);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return (float32x4_t)veorq_s32((int32x4_t)a, (int32x4_t)mask);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_abs(simd_f_t a)
{
#ifdef __AVX512F__
  return _mm512_andnot_ps(_mm512_set1_ps(-0.0f), a);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_andnot_ps(_mm256_set1_ps(-0.0f), a);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_andnot_ps(_mm_set1_ps(-0.0f), a);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vabsq_f32(a);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_interleave_low(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  return _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      b);
#else  /* __AVX512F__ */
  float reg_a[8], reg_b[8], reg_ret[8];
  srsran_simd_f_storeu(reg_a, a);
  srsran_simd_f_storeu(reg_b, b);

  for (unsigned i = 0, j = 0; i != 4; ++i) {
    reg_ret[j++] = reg_a[i];
    reg_ret[j++] = reg_b[i];
  }

  return srsran_simd_f_loadu(reg_ret);
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_interleave_high(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  return _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0a, 0x1a, 0x0b, 0x1b, 0x0c, 0x1c, 0x0d, 0x1d, 0x0e, 0x1e, 0x0f, 0x1f),
      b);
#else  /* __AVX512F__ */
  float reg_a[SRSRAN_SIMD_F_SIZE], reg_b[SRSRAN_SIMD_F_SIZE], reg_ret[SRSRAN_SIMD_F_SIZE];
  srsran_simd_f_storeu(reg_a, a);
  srsran_simd_f_storeu(reg_b, b);

  for (unsigned i = 0, j = 0; i != SRSRAN_SIMD_F_SIZE / 2; ++i) {
    reg_ret[j++] = reg_a[i + SRSRAN_SIMD_F_SIZE / 2];
    reg_ret[j++] = reg_b[i + SRSRAN_SIMD_F_SIZE / 2];
  }

  return srsran_simd_f_loadu(reg_ret);
#endif /* __AVX512F__ */
}

inline void srsran_simd_f_fprintf(std::FILE* stream, simd_f_t a)
{
  float x[SRSRAN_SIMD_F_SIZE];

  srsran_simd_f_storeu(x, a);

  std::fprintf(stream, "[");
  for (float f : x) {
    std::fprintf(stream, "%+2.5f, ", f);
  }
  std::fprintf(stream, "];\n");
}

#endif /* SRSRAN_SIMD_F_SIZE */

#if SRSRAN_SIMD_CF_SIZE

#ifdef __ARM_NEON
using simd_cf_t = float32x4x2_t;
#else
struct simd_cf_t {
  simd_f_t re;
  simd_f_t im;
};
#endif

///
/// Complex Single precision Floating point functions.
///

inline simd_cf_t srsran_simd_cfi_load(const cf_t* ptr)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  __m512 in1 = _mm512_load_ps(reinterpret_cast<const float*>(ptr));
  __m512 in2 = _mm512_load_ps(reinterpret_cast<const float*>(ptr + SRSRAN_SIMD_CF_SIZE / 2));
  ret.re     = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e),
      in2);
  ret.im = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f),
      in2);
#else /* __AVX512F__ */
#ifdef __AVX2__
  __m256 in1 = _mm256_permute_ps(_mm256_load_ps(reinterpret_cast<const float*>(ptr)), 0b11011000);
  __m256 in2 = _mm256_permute_ps(_mm256_load_ps(reinterpret_cast<const float*>(ptr + 4)), 0b11011000);
  ret.re     = _mm256_unpacklo_ps(in1, in2);
  ret.im     = _mm256_unpackhi_ps(in1, in2);

  // Permute for AVX registers (reorders data across 128-bit registers).
  const __m256i idx = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  ret.re            = _mm256_permutevar8x32_ps(ret.re, idx);
  ret.im            = _mm256_permutevar8x32_ps(ret.im, idx);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  __m128 i1 = _mm_load_ps(reinterpret_cast<const float*>(ptr));
  __m128 i2 = _mm_load_ps(reinterpret_cast<const float*>(ptr + 2));
  ret.re    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(2, 0, 2, 0));
  ret.im    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(3, 1, 3, 1));
#else
#ifdef __ARM_NEON
  ret        = vld2q_f32(reinterpret_cast<const float*>(ptr));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cfi_loadu(const cf_t* ptr)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  __m512 in1 = _mm512_loadu_ps(reinterpret_cast<const float*>(ptr));
  __m512 in2 = _mm512_loadu_ps(reinterpret_cast<const float*>(ptr + SRSRAN_SIMD_CF_SIZE / 2));
  ret.re     = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e),
      in2);
  ret.im = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f),
      in2);
#else /* __AVX512F__ */
#ifdef __AVX2__
  __m256 in1        = _mm256_permute_ps(_mm256_loadu_ps(reinterpret_cast<const float*>(ptr)), 0b11011000);
  __m256 in2        = _mm256_permute_ps(_mm256_loadu_ps(reinterpret_cast<const float*>(ptr + 4)), 0b11011000);
  ret.re            = _mm256_unpacklo_ps(in1, in2);
  ret.im            = _mm256_unpackhi_ps(in1, in2);

  // Permute for AVX registers (reorders data across 128-bit registers).
  const __m256i idx = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  ret.re            = _mm256_permutevar8x32_ps(ret.re, idx);
  ret.im            = _mm256_permutevar8x32_ps(ret.im, idx);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  __m128 i1 = _mm_loadu_ps(reinterpret_cast<const float*>(ptr));
  __m128 i2 = _mm_loadu_ps(reinterpret_cast<const float*>(ptr + 2));
  ret.re    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(2, 0, 2, 0));
  ret.im    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(3, 1, 3, 1));
#else
#ifdef __ARM_NEON
  ret        = vld2q_f32(reinterpret_cast<const float*>(ptr));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_load(const float* re, const float* im)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_load_ps(re);
  ret.im = _mm512_load_ps(im);
#else /* __AVX512F__ */
#ifdef __AVX2__
  ret.re            = _mm256_load_ps(re);
  ret.im            = _mm256_load_ps(im);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  ret.re    = _mm_load_ps(re);
  ret.im    = _mm_load_ps(im);
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  ret.val[0] = vld1q_f32(re);
  ret.val[1] = vld1q_f32(im);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_loadu(const float* re, const float* im)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_loadu_ps(re);
  ret.im = _mm512_loadu_ps(im);
#else /* __AVX512F__ */
#ifdef __AVX2__
  ret.re            = _mm256_loadu_ps(re);
  ret.im            = _mm256_loadu_ps(im);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  ret.re    = _mm_loadu_ps(re);
  ret.im    = _mm_loadu_ps(im);
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  ret.val[0] = vld1q_f32(re);
  ret.val[1] = vld1q_f32(im);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline void srsran_simd_cfi_store(cf_t* ptr, simd_cf_t simdreg)
{
#ifdef __AVX512F__
  __m512 s1 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      simdreg.im);
  __m512 s2 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0a, 0x1a, 0x0b, 0x1b, 0x0c, 0x1c, 0x0d, 0x1d, 0x0e, 0x1e, 0x0f, 0x1f),
      simdreg.im);
  _mm512_store_ps(reinterpret_cast<float*>(ptr), s1);
  _mm512_store_ps(reinterpret_cast<float*>(ptr + 8), s2);
#else /* __AVX512F__ */
#ifdef __AVX2__
  // Permute for AVX registers (reorders data across 128-bit registers).
  const __m256i idx = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);
  simdreg.re        = _mm256_permutevar8x32_ps(simdreg.re, idx);
  simdreg.im        = _mm256_permutevar8x32_ps(simdreg.im, idx);

  __m256 out1 = _mm256_permute_ps(simdreg.re, 0b11011000);
  __m256 out2 = _mm256_permute_ps(simdreg.im, 0b11011000);
  _mm256_store_ps(reinterpret_cast<float*>(ptr), _mm256_unpacklo_ps(out1, out2));
  _mm256_store_ps(reinterpret_cast<float*>(ptr + 4), _mm256_unpackhi_ps(out1, out2));
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_store_ps(reinterpret_cast<float*>(ptr), _mm_unpacklo_ps(simdreg.re, simdreg.im));
  _mm_store_ps(reinterpret_cast<float*>(ptr + 2), _mm_unpackhi_ps(simdreg.re, simdreg.im));
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  vst2q_f32(reinterpret_cast<float*>(ptr), simdreg);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_cfi_storeu(cf_t* ptr, simd_cf_t simdreg)
{
#ifdef __AVX512F__
  __m512 s1 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      simdreg.im);
  __m512 s2 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0a, 0x1a, 0x0b, 0x1b, 0x0c, 0x1c, 0x0d, 0x1d, 0x0e, 0x1e, 0x0f, 0x1f),
      simdreg.im);
  _mm512_storeu_ps(reinterpret_cast<float*>(ptr), s1);
  _mm512_storeu_ps(reinterpret_cast<float*>(ptr + 8), s2);
#else /* __AVX512F__ */
#ifdef __AVX2__
  // Permute for AVX registers (reorders data across 128-bit registers).
  const __m256i idx = _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7);
  simdreg.re        = _mm256_permutevar8x32_ps(simdreg.re, idx);
  simdreg.im        = _mm256_permutevar8x32_ps(simdreg.im, idx);

  __m256 out1 = _mm256_permute_ps(simdreg.re, 0b11011000);
  __m256 out2 = _mm256_permute_ps(simdreg.im, 0b11011000);
  _mm256_storeu_ps(reinterpret_cast<float*>(ptr), _mm256_unpacklo_ps(out1, out2));
  _mm256_storeu_ps(reinterpret_cast<float*>(ptr + 4), _mm256_unpackhi_ps(out1, out2));
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_storeu_ps(reinterpret_cast<float*>(ptr), _mm_unpacklo_ps(simdreg.re, simdreg.im));
  _mm_storeu_ps(reinterpret_cast<float*>(ptr + 2), _mm_unpackhi_ps(simdreg.re, simdreg.im));
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  vst2q_f32(reinterpret_cast<float*>(ptr), simdreg);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_cf_store(float* re, float* im, simd_cf_t simdreg)
{
#ifdef __AVX512F__
  _mm512_store_ps(re, simdreg.re);
  _mm512_store_ps(im, simdreg.im);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_store_ps(re, simdreg.re);
  _mm256_store_ps(im, simdreg.im);
#else /* __AVX512F__ */
#ifdef __SSE4_1__
  _mm_store_ps(re, simdreg.re);
  _mm_store_ps(im, simdreg.im);
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  vst1q_f32(re, simdreg.val[0]);
  vst1q_f32(im, simdreg.val[1]);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_cf_storeu(float* re, float* im, simd_cf_t simdreg)
{
#ifdef __AVX512F__
  _mm512_storeu_ps(re, simdreg.re);
  _mm512_storeu_ps(im, simdreg.im);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_storeu_ps(re, simdreg.re);
  _mm256_storeu_ps(im, simdreg.im);
#else /* __AVX512F__ */
#ifdef __SSE4_1__
  _mm_storeu_ps(re, simdreg.re);
  _mm_storeu_ps(im, simdreg.im);
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  vst1q_f32(re, simdreg.val[0]);
  vst1q_f32(im, simdreg.val[1]);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_cf_re(simd_cf_t in)
{
#ifdef __ARM_NEON
  simd_f_t out = in.val[0];
#else
  simd_f_t out      = in.re;
#endif /* __ARM_NEON */
  return out;
}

inline simd_f_t srsran_simd_cf_im(simd_cf_t in)
{
#ifdef __ARM_NEON
  simd_f_t out = in.val[1];
#else
  simd_f_t out      = in.im;
#endif /* __ARM_NEON */
  return out;
}

inline simd_cf_t srsran_simd_cf_set1(cf_t x)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_set1_ps(x.real());
  ret.im = _mm512_set1_ps(x.imag());
#else /* __AVX512F__ */
#ifdef __AVX2__
  ret.re            = _mm256_set1_ps(x.real());
  ret.im            = _mm256_set1_ps(x.imag());
#else
#ifdef __SSE4_1__
  ret.re            = _mm_set1_ps(x.real());
  ret.im            = _mm_set1_ps(x.imag());
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  ret.val[0]        = vdupq_n_f32(x.real());
  ret.val[1]        = vdupq_n_f32(x.imag());
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_prod(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_sub_ps(_mm512_mul_ps(a.re, b.re), _mm512_mul_ps(a.im, b.im));
  ret.im = _mm512_add_ps(_mm512_mul_ps(a.re, b.im), _mm512_mul_ps(a.im, b.re));
#else /* __AVX512F__ */
#ifdef __AVX2__
#ifdef __FMA__
  ret.re            = _mm256_fmsub_ps(a.re, b.re, _mm256_mul_ps(a.im, b.im));
  ret.im            = _mm256_fmadd_ps(a.re, b.im, _mm256_mul_ps(a.im, b.re));
#else  /* __FMA__ */
  ret.re = _mm256_sub_ps(_mm256_mul_ps(a.re, b.re), _mm256_mul_ps(a.im, b.im));
  ret.im = _mm256_add_ps(_mm256_mul_ps(a.re, b.im), _mm256_mul_ps(a.im, b.re));
#endif /* __FMA__ */
#else
#ifdef __SSE4_1__
  ret.re            = _mm_sub_ps(_mm_mul_ps(a.re, b.re), _mm_mul_ps(a.im, b.im));
  ret.im            = _mm_add_ps(_mm_mul_ps(a.re, b.im), _mm_mul_ps(a.im, b.re));
#else
#ifdef __ARM_NEON
  ret.val[0]        = vsubq_f32(vmulq_f32(a.val[0], b.val[0]), vmulq_f32(a.val[1], b.val[1]));
  ret.val[1]        = vaddq_f32(vmulq_f32(a.val[0], b.val[1]), vmulq_f32(a.val[1], b.val[0]));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_conjprod(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_add_ps(_mm512_mul_ps(a.re, b.re), _mm512_mul_ps(a.im, b.im));
  ret.im = _mm512_sub_ps(_mm512_mul_ps(a.im, b.re), _mm512_mul_ps(a.re, b.im));
#else /* __AVX512F__ */
#ifdef __AVX2__
  ret.re            = _mm256_add_ps(_mm256_mul_ps(a.re, b.re), _mm256_mul_ps(a.im, b.im));
  ret.im            = _mm256_sub_ps(_mm256_mul_ps(a.im, b.re), _mm256_mul_ps(a.re, b.im));
#else /* __AVX2__ */
#ifdef __SSE4_1__
  ret.re            = _mm_add_ps(_mm_mul_ps(a.re, b.re), _mm_mul_ps(a.im, b.im));
  ret.im            = _mm_sub_ps(_mm_mul_ps(a.im, b.re), _mm_mul_ps(a.re, b.im));
#else
#ifdef __ARM_NEON
  ret.val[0]        = vaddq_f32(vmulq_f32(a.val[0], b.val[0]), vmulq_f32(a.val[1], b.val[1]));
  ret.val[1]        = vsubq_f32(vmulq_f32(a.val[1], b.val[0]), vmulq_f32(a.val[0], b.val[1]));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_add(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_add_ps(a.re, b.re);
  ret.im = _mm512_add_ps(a.im, b.im);
#else /* __AVX512F__ */
#ifdef __AVX2__
  ret.re            = _mm256_add_ps(a.re, b.re);
  ret.im            = _mm256_add_ps(a.im, b.im);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  ret.re            = _mm_add_ps(a.re, b.re);
  ret.im            = _mm_add_ps(a.im, b.im);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  ret.val[0]        = vaddq_f32(a.val[0], b.val[0]);
  ret.val[1]        = vaddq_f32(a.val[1], b.val[1]);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_sub(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_sub_ps(a.re, b.re);
  ret.im = _mm512_sub_ps(a.im, b.im);
#else /* __AVX512F__ */
#ifdef __AVX2__
  ret.re            = _mm256_sub_ps(a.re, b.re);
  ret.im            = _mm256_sub_ps(a.im, b.im);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  ret.re            = _mm_sub_ps(a.re, b.re);
  ret.im            = _mm_sub_ps(a.im, b.im);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  ret.val[0]        = vsubq_f32(a.val[0], b.val[0]);
  ret.val[1]        = vsubq_f32(a.val[1], b.val[1]);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_f_t srsran_simd_cf_norm_sq(simd_cf_t a)
{
  return srsran_simd_f_fma(
      srsran_simd_f_mul(srsran_simd_cf_re(a), srsran_simd_cf_re(a)), srsran_simd_cf_im(a), srsran_simd_cf_im(a));
}

inline simd_cf_t srsran_simd_cf_mul(simd_cf_t a, simd_f_t b)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_mul_ps(a.re, b);
  ret.im = _mm512_mul_ps(a.im, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  ret.re            = _mm256_mul_ps(a.re, b);
  ret.im            = _mm256_mul_ps(a.im, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  ret.re            = _mm_mul_ps(a.re, b);
  ret.im            = _mm_mul_ps(a.im, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  ret.val[0]        = vmulq_f32(a.val[0], b);
  ret.val[1]        = vmulq_f32(a.val[1], b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_rcp(simd_cf_t a)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  simd_f_t a2re     = _mm512_mul_ps(a.re, a.re);
  simd_f_t a2im     = _mm512_mul_ps(a.im, a.im);
  simd_f_t mod2     = _mm512_add_ps(a2re, a2im);
  simd_f_t rcp      = _mm512_rcp14_ps(mod2);
  simd_f_t neg_a_im = _mm512_xor_ps(_mm512_set1_ps(-0.0f), a.im);
  ret.re            = _mm512_mul_ps(a.re, rcp);
  ret.im            = _mm512_mul_ps(neg_a_im, rcp);
#else /* __AVX512F__ */
#ifdef __AVX2__
  simd_f_t a2re     = _mm256_mul_ps(a.re, a.re);
  simd_f_t a2im     = _mm256_mul_ps(a.im, a.im);
  simd_f_t mod2     = _mm256_add_ps(a2re, a2im);
  simd_f_t rcp      = _mm256_rcp_ps(mod2);
  simd_f_t neg_a_im = _mm256_xor_ps(_mm256_set1_ps(-0.0f), a.im);
  ret.re            = _mm256_mul_ps(a.re, rcp);
  ret.im            = _mm256_mul_ps(neg_a_im, rcp);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  simd_f_t a2re     = _mm_mul_ps(a.re, a.re);
  simd_f_t a2im     = _mm_mul_ps(a.im, a.im);
  simd_f_t mod2     = _mm_add_ps(a2re, a2im);
  simd_f_t rcp      = _mm_rcp_ps(mod2);
  simd_f_t neg_a_im = _mm_xor_ps(_mm_set1_ps(-0.0f), a.im);
  ret.re            = _mm_mul_ps(a.re, rcp);
  ret.im            = _mm_mul_ps(neg_a_im, rcp);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  simd_f_t a2re     = vmulq_f32(a.val[0], a.val[0]);
  simd_f_t a2im     = vmulq_f32(a.val[1], a.val[1]);
  simd_f_t mod2     = vaddq_f32(a2re, a2im);
  simd_f_t rcp      = vmulq_f32(vrecpeq_f32(mod2), vrecpsq_f32(vrecpeq_f32(mod2), mod2));
  simd_f_t neg_a_im = vnegq_f32(a.val[1]);
  ret.val[0]        = vmulq_f32(a.val[0], rcp);
  ret.val[1]        = vmulq_f32(neg_a_im, rcp);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_neg(simd_cf_t a)
{
  simd_cf_t ret;
#if __ARM_NEON
  ret.val[0] = srsran_simd_f_neg(a.val[0]);
  ret.val[1] = srsran_simd_f_neg(a.val[1]);
#else  /* __ARM_NEON */
  ret.re            = srsran_simd_f_neg(a.re);
  ret.im            = srsran_simd_f_neg(a.im);
#endif /* __ARM_NEON */
  return ret;
}

inline simd_cf_t srsran_simd_cf_neg_mask(simd_cf_t a, simd_f_t mask)
{
  simd_cf_t ret;
#if __ARM_NEON
  ret.val[0] = srsran_simd_f_neg_mask(a.val[0], mask);
  ret.val[1] = srsran_simd_f_neg_mask(a.val[1], mask);
#else  /* __ARM_NEON */
  ret.re            = srsran_simd_f_neg_mask(a.re, mask);
  ret.im            = srsran_simd_f_neg_mask(a.im, mask);
#endif /* __ARM_NEON */
  return ret;
}

inline simd_cf_t srsran_simd_cf_conj(simd_cf_t a)
{
  simd_cf_t ret;
#if __ARM_NEON
  ret.val[0] = a.val[0];
  ret.val[1] = srsran_simd_f_neg(a.val[1]);
#else  /* __ARM_NEON */
  ret.re            = a.re;
  ret.im            = srsran_simd_f_neg(a.im);
#endif /* __ARM_NEON */
  return ret;
}

inline simd_cf_t srsran_simd_cf_mulj(simd_cf_t a)
{
  simd_cf_t ret;
#if __ARM_NEON
  ret.val[0] = srsran_simd_f_neg(a.val[1]);
  ret.val[1] = a.val[0];
#else  /* __ARM_NEON */
  ret.re            = srsran_simd_f_neg(a.im);
  ret.im            = a.re;
#endif /* __ARM_NEON */
  return ret;
}

inline simd_cf_t srsran_simd_cf_zero()
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_setzero_ps();
  ret.im = _mm512_setzero_ps();
#else /* __AVX512F__ */
#ifdef __AVX2__
  ret.re            = _mm256_setzero_ps();
  ret.im            = _mm256_setzero_ps();
#else /* __AVX2__ */
#ifdef __SSE4_1__
  ret.re            = _mm_setzero_ps();
  ret.im            = _mm_setzero_ps();
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  ret.val[0]        = vdupq_n_f32(0);
  ret.val[1]        = vdupq_n_f32(0);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_interleave_low(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_permutex2var_ps(
      a.re,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      b.re);
  ret.im = _mm512_permutex2var_ps(
      a.im,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      b.im);
#else /* __AVX512F__ */
#ifdef __AVX2__
  __m128 a_re       = _mm256_extractf128_ps(a.re, 0);
  __m128 b_re       = _mm256_extractf128_ps(b.re, 0);

  __m128 a_im = _mm256_extractf128_ps(a.im, 0);
  __m128 b_im = _mm256_extractf128_ps(b.im, 0);

  __m128 hi_re = _mm_unpackhi_ps(a_re, b_re);
  __m128 lo_re = _mm_unpacklo_ps(a_re, b_re);

  __m128 hi_im = _mm_unpackhi_ps(a_im, b_im);
  __m128 lo_im = _mm_unpacklo_ps(a_im, b_im);

  ret.re      = _mm256_set_m128(hi_re, lo_re);
  ret.im      = _mm256_set_m128(hi_im, lo_im);
#else  /* __AVX2__ */
  cf_t reg_a[SRSRAN_SIMD_CF_SIZE], reg_b[SRSRAN_SIMD_CF_SIZE], reg_ret[SRSRAN_SIMD_CF_SIZE];
  srsran_simd_cfi_storeu(reg_a, a);
  srsran_simd_cfi_storeu(reg_b, b);

  for (unsigned i = 0, j = 0; i != SRSRAN_SIMD_CF_SIZE / 2; ++i) {
    reg_ret[j++] = reg_a[i];
    reg_ret[j++] = reg_b[i];
  }

  ret = srsran_simd_cfi_loadu(reg_ret);
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_interleave_high(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_permutex2var_ps(
      a.re,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0a, 0x1a, 0x0b, 0x1b, 0x0c, 0x1c, 0x0d, 0x1d, 0x0e, 0x1e, 0x0f, 0x1f),
      b.re);
  ret.im = _mm512_permutex2var_ps(
      a.im,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0a, 0x1a, 0x0b, 0x1b, 0x0c, 0x1c, 0x0d, 0x1d, 0x0e, 0x1e, 0x0f, 0x1f),
      b.im);
#else /* __AVX512F__ */
#ifdef __AVX2__
  __m128 a_re = _mm256_extractf128_ps(a.re, 1);
  __m128 b_re = _mm256_extractf128_ps(b.re, 1);

  __m128 a_im = _mm256_extractf128_ps(a.im, 1);
  __m128 b_im = _mm256_extractf128_ps(b.im, 1);

  __m128 hi_re = _mm_unpackhi_ps(a_re, b_re);
  __m128 lo_re = _mm_unpacklo_ps(a_re, b_re);

  __m128 hi_im = _mm_unpackhi_ps(a_im, b_im);
  __m128 lo_im = _mm_unpacklo_ps(a_im, b_im);

  ret.re = _mm256_set_m128(hi_re, lo_re);
  ret.im = _mm256_set_m128(hi_im, lo_im);
#else  /* __AVX2__ */
  cf_t reg_a[SRSRAN_SIMD_CF_SIZE], reg_b[SRSRAN_SIMD_CF_SIZE], reg_ret[SRSRAN_SIMD_CF_SIZE];
  srsran_simd_cfi_storeu(reg_a, a);
  srsran_simd_cfi_storeu(reg_b, b);

  for (unsigned i = 0, j = 0; i != SRSRAN_SIMD_CF_SIZE / 2; ++i) {
    reg_ret[j++] = reg_a[i + SRSRAN_SIMD_CF_SIZE / 2];
    reg_ret[j++] = reg_b[i + SRSRAN_SIMD_CF_SIZE / 2];
  }

  ret = srsran_simd_cfi_loadu(reg_ret);
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline void srsran_simd_cf_fprintf(std::FILE* stream, simd_cf_t a)
{
  cf_t x[SRSRAN_SIMD_CF_SIZE];

  srsran_simd_cfi_storeu(x, a);

  std::fprintf(stream, "[");
  for (auto c : x) {
    std::fprintf(stream, "%+2.5f%+2.5fi, ", c.real(), c.imag());
  }
  std::fprintf(stream, "];\n");
}

/// Adds two complex SIMD registers.
inline simd_cf_t operator+(simd_cf_t left, simd_cf_t right)
{
  return srsran_simd_cf_add(left, right);
}

/// Subtracts two complex SIMD registers.
inline simd_cf_t operator-(simd_cf_t left, simd_cf_t right)
{
  return srsran_simd_cf_sub(left, right);
}

/// Multiplies two complex SIMD registers.
inline simd_cf_t operator*(simd_cf_t left, simd_cf_t right)
{
  return srsran_simd_cf_prod(left, right);
}

/// Multiplies a complex and a real SIMD registers.
inline simd_cf_t operator*(simd_cf_t left, simd_f_t right)
{
  return srsran_simd_cf_mul(left, right);
}

/// Multiplies two complex SIMD registers.
inline simd_cf_t& operator*=(simd_cf_t& left, const simd_cf_t& right)
{
  left = srsran_simd_cf_prod(left, right);
  return left;
}

/// Multiplies a complex SIMD register by a real literal.
inline simd_cf_t& operator*=(simd_cf_t& left, float right)
{
  left = srsran_simd_cf_mul(left, srsran_simd_f_set1(right));
  return left;
}

/// Multiplies a complex and a real SIMD registers.
inline simd_cf_t& operator*=(simd_cf_t& left, simd_f_t right)
{
  left = srsran_simd_cf_mul(left, right);
  return left;
}

/// Subtract two complex SIMD registers.
inline simd_cf_t& operator-=(simd_cf_t& left, const simd_cf_t& right)
{
  left = srsran_simd_cf_sub(left, right);
  return left;
}

/// Add two complex SIMD registers.
inline simd_cf_t& operator+=(simd_cf_t& left, const simd_cf_t& right)
{
  left = srsran_simd_cf_add(left, right);
  return left;
}

/// Negate a complex SIMD register.
inline simd_cf_t operator-(simd_cf_t value)
{
  return srsran_simd_cf_neg(value);
}

#endif /* SRSRAN_SIMD_CF_SIZE */

#if SRSRAN_SIMD_I_SIZE

#ifdef __AVX512F__
using simd_i_t   = __m512i;
using simd_sel_t = __mmask16;
#else /* __AVX512F__ */
#ifdef __AVX2__
using simd_i_t   = __m256i;
using simd_sel_t = __m256;
#else /* __AVX2__ */
#ifdef __SSE4_1__
using simd_i_t   = __m128i;
using simd_sel_t = __m128;
#else /* __AVX2__ */
#ifdef __ARM_NEON
using simd_i_t   = int32x4_t;
using simd_sel_t = uint32x4_t;
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */

inline simd_i_t srsran_simd_i_load(const int32_t* x)
{
#ifdef __AVX512F__
  return _mm512_load_epi32(reinterpret_cast<const __m512i*>(x));
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_load_si256(reinterpret_cast<const __m256i*>(x));
#else
#ifdef __SSE4_1__
  return _mm_load_si128(reinterpret_cast<const __m128i*>(x));
#else
#ifdef __ARM_NEON
  return vld1q_s32(reinterpret_cast<const int*>(x));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_i_store(int32_t* x, simd_i_t reg)
{
#ifdef __AVX512F__
  _mm512_store_epi32(reinterpret_cast<__m512i*>(x), reg);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_store_si256(reinterpret_cast<__m256i*>(x), reg);
#else
#ifdef __SSE4_1__
  _mm_store_si128(reinterpret_cast<__m128i*>(x), reg);
#else
#ifdef __ARM_NEON
  vst1q_s32(reinterpret_cast<int*>(x), reg);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_i_t srsran_simd_i_set1(int x)
{
#ifdef __AVX512F__
  return _mm512_set1_epi32(x);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_set1_epi32(x);
#else
#ifdef __SSE4_1__
  return _mm_set1_epi32(x);
#else
#ifdef __ARM_NEON
  return vdupq_n_s32(x);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_i_t srsran_simd_i_add(simd_i_t a, simd_i_t b)
{
#ifdef __AVX512F__
  return _mm512_add_epi32(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_add_epi32(a, b);
#else
#ifdef __SSE4_1__
  return _mm_add_epi32(a, b);
#else
#ifdef __ARM_NEON
  return vaddq_s32(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_i_t srsran_simd_i_mul(simd_i_t a, simd_i_t b)
{
#ifdef __AVX512F__
  return _mm512_mullo_epi32(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_mullo_epi32(a, b);
#else
#ifdef __SSE4_1__
  return _mm_mul_epi32(a, b);
#else
#ifdef __ARM_NEON
  return vmulq_s32(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_i_t srsran_simd_i_and(simd_i_t a, simd_i_t b)
{
#ifdef __AVX512F__
  return _mm512_and_si512(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_and_si256(a, b);
#else
#ifdef __SSE4_1__
  return _mm_and_si128(a, b);
#else
#ifdef __ARM_NEON
  return vandq_s32(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_sel_t srsran_simd_sel_set_ones()
{
#ifdef __AVX512F__
  return ~static_cast<simd_sel_t>(0);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_castsi256_ps(_mm256_set1_epi32(0xffffffff));
#else
#ifdef __SSE4_1__
  return _mm_castsi128_ps(_mm_set1_epi32(0xffffffff));
#else
#ifdef __ARM_NEON
  return vdupq_n_u32(0xffffffff);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_sel_t srsran_simd_sel_and(simd_sel_t a, simd_sel_t b)
{
#ifdef __AVX512F__
  return _kand_mask16(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_and_ps(a, b);
#else
#ifdef __SSE4_1__
  return _mm_and_ps(a, b);
#else
#ifdef __ARM_NEON
  return vandq_u32(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_sel_t srsran_simd_f_max(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  return _mm512_cmp_ps_mask(a, b, _CMP_GT_OS);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_cmp_ps(a, b, _CMP_GT_OS);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return (simd_sel_t)_mm_cmpgt_ps(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vcgtq_f32(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_sel_t srsran_simd_f_min(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  return _mm512_cmp_ps_mask(a, b, _CMP_LT_OS);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_cmp_ps(a, b, _CMP_LT_OS);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return (simd_sel_t)_mm_cmplt_ps(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vcltq_f32(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_f_t srsran_simd_f_select(simd_f_t a, simd_f_t b, simd_sel_t selector)
{
#ifdef __AVX512F__
  return _mm512_mask_blend_ps(selector, (__m512)a, (__m512)b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_blendv_ps(a, b, selector);
#else
#ifdef __SSE4_1__
  return _mm_blendv_ps(a, b, selector);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vbslq_f32(selector, b, a);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_cf_t srsran_simd_cf_select(simd_cf_t a, simd_cf_t b, simd_sel_t selector)
{
  simd_cf_t ret;
#ifdef __AVX512F__
  ret.re = _mm512_mask_blend_ps(selector, a.re, b.re);
  ret.im = _mm512_mask_blend_ps(selector, a.im, b.im);
#else /* __AVX512F__ */
#ifdef __AVX2__
  ret.re = _mm256_blendv_ps(a.re, b.re, selector);
  ret.im = _mm256_blendv_ps(a.im, b.im, selector);
#else
#ifdef __SSE4_1__
  ret.re = _mm_blendv_ps(a.re, b.re, selector);
  ret.im = _mm_blendv_ps(a.im, b.im, selector);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  ret.val[0] = vbslq_f32(selector, b.val[0], a.val[0]);
  ret.val[1] = vbslq_f32(selector, b.val[1], a.val[1]);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_cf_t srsran_simd_cf_add_sel(simd_cf_t a, simd_cf_t b, simd_sel_t sel)
{
  return srsran_simd_cf_select(a, srsran_simd_cf_add(a, b), sel);
}

inline simd_f_t srsran_simd_f_add_sel(simd_f_t a, simd_f_t b, simd_sel_t sel)
{
  return srsran_simd_f_select(a, srsran_simd_f_add(a, b), sel);
}

inline simd_i_t srsran_simd_i_select(simd_i_t a, simd_i_t b, simd_sel_t selector)
{
#ifdef __AVX512F__
  return (__m512i)_mm512_mask_blend_ps(selector, (__m512)a, (__m512)b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return (__m256i)_mm256_blendv_ps((__m256)a, (__m256)b, selector);
#else
#ifdef __SSE4_1__
  return (__m128i)_mm_blendv_ps((__m128)a, (__m128)b, selector);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vbslq_s32(selector, b, a);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

#endif /* SRSRAN_SIMD_I_SIZE */

#if SRSRAN_SIMD_S_SIZE

#ifdef __AVX512F__
using simd_s_t = __m512i;
#else /* __AVX512F__ */
#ifdef __AVX2__
using simd_s_t = __m256i;
#else /* __AVX2__ */
#ifdef __SSE4_1__
using simd_s_t = __m128i;
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
using simd_s_t = int16x8_t;
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */

inline simd_s_t srsran_simd_s_load(const int16_t* ptr)
{
#ifdef __AVX512F__
  return _mm512_load_si512(ptr);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_load_si128(reinterpret_cast<const __m128i*>(ptr));
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vld1q_s16(ptr);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_s_t srsran_simd_s_loadu(const int16_t* ptr)
{
#ifdef __AVX512F__
  return _mm512_loadu_si512(ptr);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vld1q_s16(ptr);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_s_store(int16_t* ptr, simd_s_t simdreg)
{
#ifdef __AVX512F__
  _mm512_store_si512(ptr, simdreg);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_store_si256(reinterpret_cast<__m256i*>(ptr), simdreg);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_store_si128(reinterpret_cast<__m128i*>(ptr), simdreg);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  vst1q_s16(ptr, simdreg);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_s_storeu(int16_t* ptr, simd_s_t simdreg)
{
#ifdef __AVX512F__
  _mm512_storeu_si512(ptr, simdreg);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr), simdreg);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_storeu_si128(reinterpret_cast<__m128i*>(ptr), simdreg);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  vst1q_s16(ptr, simdreg);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}
inline simd_s_t srsran_simd_s_zero()
{
#ifdef __AVX512F__
  return _mm512_setzero_si512();
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_setzero_si256();
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_setzero_si128();
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vdupq_n_s16(0);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_s_t srsran_simd_s_mul(simd_s_t a, simd_s_t b)
{
#ifdef __AVX512F__
  return _mm512_mullo_epi16(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_mullo_epi16(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_mullo_epi16(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vmulq_s16(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_s_t srsran_simd_s_neg(simd_s_t a, simd_s_t b)
{
#ifdef __AVX512F__
  __m256i a0 = _mm512_extracti64x4_epi64(a, 0);
  __m256i a1 = _mm512_extracti64x4_epi64(a, 1);
  __m256i b0 = _mm512_extracti64x4_epi64(b, 0);
  __m256i b1 = _mm512_extracti64x4_epi64(b, 1);
  __m256i r0 = _mm256_sign_epi16(a0, b0);
  __m256i r1 = _mm256_sign_epi16(a1, b1);
  return _mm512_inserti64x4(_mm512_broadcast_i64x4(r0), r1, 1);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_sign_epi16(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_sign_epi16(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  // Taken and modified from sse2neon.h licensed under MIT.
  // Source: https://github.com/DLTcollab/sse2neon
  int16x8_t zero = vdupq_n_s16(0);
  // Signed shift right: faster than vclt.
  // (b < 0) ? 0xffff : 0
  uint16x8_t ltMask = vreinterpretq_u16_s16(vshrq_n_s16(b, 15));
  // (b == 0) ? 0xffff : 0
  int16x8_t zeroMask = vreinterpretq_s16_u16(vceqq_s16(b, zero));
  // -a
  int16x8_t neg = vnegq_s16(a);
  // Bitwise select either a or neg based on ltMask.
  int16x8_t masked = vbslq_s16(ltMask, neg, a);
  // res = masked & (~zeroMask)
  int16x8_t res = vbicq_s16(masked, zeroMask);
  return res;
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_s_t srsran_simd_s_add(simd_s_t a, simd_s_t b)
{
#ifdef __AVX512F__
  return _mm512_add_epi16(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_add_epi16(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_add_epi16(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vaddq_s16(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_s_t srsran_simd_s_sub(simd_s_t a, simd_s_t b)
{
#ifdef __AVX512F__
  return _mm512_sub_epi16(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_sub_epi16(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_sub_epi16(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vsubq_s16(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_s_t srsran_simd_s_set1(int16_t x)
{
#ifdef __AVX512F__
  return _mm512_set1_epi16(x);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_set1_epi16(x);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_set1_epi16(x);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vdupq_n_s16(x);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

#endif /* SRSRAN_SIMD_S_SIZE */

#if SRSRAN_SIMD_C16_SIZE

#ifdef __AVX512F__
struct simd_c16_t {
  __m512i re;
  __m512i im;
};
#else /* __AVX512F__ */
#ifdef __AVX2__
struct simd_c16_t {
  __m256i re;
  __m256i im;
};
#else
#ifdef __SSE4_1__
struct simd_c16_t {
  __m128i re;
  __m128i im;
};
#else
#ifdef __ARM_NEON
struct simd_c16_t {
  int16x8x2_t m128;
};
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */

///
/// Fixed point precision (16-bit) functions.
///

inline simd_c16_t srsran_simd_c16i_load(const c16_t* ptr)
{
  simd_c16_t ret;
#ifdef __AVX512F__
  __m512i in1 = _mm512_load_si512(reinterpret_cast<const __m512i*>(ptr));
  __m512i in2 = _mm512_load_si512(reinterpret_cast<const __m512i*>(ptr + 8));
  ret.re      = _mm512_mask_blend_epi16(
      0xaaaaaaaa, in1, _mm512_shufflelo_epi16(_mm512_shufflehi_epi16(in2, 0b10100000), 0b10100000));
  ret.im = _mm512_mask_blend_epi16(
      0xaaaaaaaa, _mm512_shufflelo_epi16(_mm512_shufflehi_epi16(in1, 0b11110101), 0b11110101), in2);
#else /* __AVX2__ */
#ifdef __AVX2__
  __m256i in1 = _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));
  __m256i in2 = _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr + 8));
  ret.re =
      _mm256_blend_epi16(in1, _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(in2, 0b10100000), 0b10100000), 0b10101010);
  ret.im =
      _mm256_blend_epi16(_mm256_shufflelo_epi16(_mm256_shufflehi_epi16(in1, 0b11110101), 0b11110101), in2, 0b10101010);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  __m128i in1 = _mm_load_si128(reinterpret_cast<const __m128i*>(ptr));
  __m128i in2 = _mm_load_si128(reinterpret_cast<const __m128i*>(ptr + 8));
  ret.re      = _mm_blend_epi16(in1, _mm_shufflelo_epi16(_mm_shufflehi_epi16(in2, 0b10100000), 0b10100000), 0b10101010);
  ret.im      = _mm_blend_epi16(_mm_shufflelo_epi16(_mm_shufflehi_epi16(in1, 0b11110101), 0b11110101), in2, 0b10101010);
#else /* __SSE4_1__*/
#ifdef __ARM_NEON
  ret.m128     = vld2q_s16(reinterpret_cast<const int16_t*>(ptr));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

inline simd_c16_t srsran_simd_c16_load(const int16_t* re, const int16_t* im)
{
  simd_c16_t ret;
#ifdef __AVX2__
  ret.re = _mm256_load_si256(reinterpret_cast<const __m256i*>(re));
  ret.im = _mm256_load_si256(reinterpret_cast<const __m256i*>(im));
#else
#ifdef __SSE4_1__
  ret.re        = _mm_load_si128(reinterpret_cast<const __m128i*>(re));
  ret.im        = _mm_load_si128(reinterpret_cast<const __m128i*>(im));
#else /* __SSE4_1__*/
#ifdef __ARM_NEON
  ret.m128.val[0] = vld1q_s16(re);
  ret.m128.val[1] = vld1q_s16(im);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
  return ret;
}

inline simd_c16_t srsran_simd_c16_loadu(const int16_t* re, const int16_t* im)
{
  simd_c16_t ret;
#ifdef __AVX2__
  ret.re = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(re));
  ret.im = _mm256_loadu_si256(reinterpret_cast<const__m256i*>(im));
#else
#ifdef __SSE4_1__
  ret.re        = _mm_loadu_si128(reinterpret_cast<const __m128i*>(re));
  ret.im        = _mm_loadu_si128(reinterpret_cast<const __m128i*>(im));
#else /* __SSE4_1__*/
#ifdef __ARM_NEON
  ret.m128.val[0] = vld1q_s16(re);
  ret.m128.val[1] = vld1q_s16(im);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
  return ret;
}

inline void srsran_simd_c16i_store(c16_t* ptr, simd_c16_t simdreg)
{
#ifdef __AVX2__
  __m256i re_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.re, 0b10110001), 0b10110001);
  __m256i im_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.im, 0b10110001), 0b10110001);
  _mm256_store_si256(reinterpret_cast<__m256i*>(ptr), _mm256_blend_epi16(simdreg.re, im_sw, 0b10101010));
  _mm256_store_si256(reinterpret_cast<__m256i*>(ptr + 8), _mm256_blend_epi16(re_sw, simdreg.im, 0b10101010));
#else
#ifdef __SSE4_1__
  __m128i re_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.re, 0b10110001), 0b10110001);
  __m128i im_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.im, 0b10110001), 0b10110001);
  _mm_store_si128(reinterpret_cast<__m128i*>(ptr), _mm_blend_epi16(simdreg.re, im_sw, 0b10101010));
  _mm_store_si128(reinterpret_cast<__m128i*>(ptr + 8), _mm_blend_epi16(re_sw, simdreg.im, 0b10101010));
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  vst2q_s16(reinterpret_cast<int16_t*>(ptr), simdreg.m128);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
}

inline void srsran_simd_c16i_storeu(c16_t* ptr, simd_c16_t simdreg)
{
#ifdef __AVX2__
  __m256i re_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.re, 0b10110001), 0b10110001);
  __m256i im_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.im, 0b10110001), 0b10110001);
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr), _mm256_blend_epi16(simdreg.re, im_sw, 0b10101010));
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr + 8), _mm256_blend_epi16(re_sw, simdreg.im, 0b10101010));
#else
#ifdef __SSE4_1__
  __m128i re_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.re, 0b10110001), 0b10110001);
  __m128i im_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.im, 0b10110001), 0b10110001);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(ptr), _mm_blend_epi16(simdreg.re, im_sw, 0b10101010));
  _mm_storeu_si128(reinterpret_cast<__m128i*>(ptr + 8), _mm_blend_epi16(re_sw, simdreg.im, 0b10101010));
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  vst2q_s16(reinterpret_cast<int16_t*>(ptr), simdreg.m128);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
}

inline void srsran_simd_c16_store(int16_t* re, int16_t* im, simd_c16_t simdreg)
{
#ifdef __AVX2__
  _mm256_store_si256(reinterpret_cast<__m256i*>(re), simdreg.re);
  _mm256_store_si256(reinterpret_cast<__m256i*>(im), simdreg.im);
#else
#ifdef __SSE4_1__
  _mm_store_si128(reinterpret_cast<__m128i*>(re), simdreg.re);
  _mm_store_si128(reinterpret_cast<__m128i*>(im), simdreg.im);
#else
#ifdef __ARM_NEON
  vst1q_s16(re, simdreg.m128.val[0]);
  vst1q_s16(im, simdreg.m128.val[1]);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
}

inline void srsran_simd_c16_storeu(int16_t* re, int16_t* im, simd_c16_t simdreg)
{
#ifdef __AVX2__
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(re), simdreg.re);
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(im), simdreg.im);
#else
#ifdef __SSE4_1__
  _mm_storeu_si128(reinterpret_cast<__m128i*>(re), simdreg.re);
  _mm_storeu_si128(reinterpret_cast<__m128i*>(im), simdreg.im);
#else
#ifdef __ARM_NEON
  vst1q_s16(re, simdreg.m128.val[0]);
  vst1q_s16(im, simdreg.m128.val[1]);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
}

inline simd_c16_t srsran_simd_c16_prod(simd_c16_t a, simd_c16_t b)
{
  simd_c16_t ret;
#ifdef __AVX2__
  ret.re = _mm256_sub_epi16(_mm256_mulhrs_epi16(a.re, _mm256_slli_epi16(b.re, 1)),
                            _mm256_mulhrs_epi16(a.im, _mm256_slli_epi16(b.im, 1)));
  ret.im = _mm256_add_epi16(_mm256_mulhrs_epi16(a.re, _mm256_slli_epi16(b.im, 1)),
                            _mm256_mulhrs_epi16(a.im, _mm256_slli_epi16(b.re, 1)));
#else
#ifdef __SSE4_1__
  ret.re =
      _mm_sub_epi16(_mm_mulhrs_epi16(a.re, _mm_slli_epi16(b.re, 1)), _mm_mulhrs_epi16(a.im, _mm_slli_epi16(b.im, 1)));
  ret.im =
      _mm_add_epi16(_mm_mulhrs_epi16(a.re, _mm_slli_epi16(b.im, 1)), _mm_mulhrs_epi16(a.im, _mm_slli_epi16(b.re, 1)));
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
  return ret;
}

inline simd_c16_t srsran_simd_c16_add(simd_c16_t a, simd_c16_t b)
{
  simd_c16_t ret;
#ifdef __AVX2__
  ret.re = _mm256_add_epi16(a.re, b.re);
  ret.im = _mm256_add_epi16(a.im, b.im);
#else
#ifdef __SSE4_1__
  ret.re     = _mm_add_epi16(a.re, b.re);
  ret.im     = _mm_add_epi16(a.im, b.im);
#else
#ifdef __ARM_NEON
  ret.m128.val[0] = vaddq_s16(a.m128.val[0], a.m128.val[0]);
  ret.m128.val[1] = vaddq_s16(a.m128.val[1], a.m128.val[1]);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
  return ret;
}

inline simd_c16_t srsran_simd_c16_zero()
{
  simd_c16_t ret;
#ifdef __AVX2__
  ret.re = _mm256_setzero_si256();
  ret.im = _mm256_setzero_si256();
#else
#ifdef __SSE4_1__
  ret.re     = _mm_setzero_si128();
  ret.im     = _mm_setzero_si128();
#else
#ifdef __ARM_NEON
  ret.m128.val[0] = vdupq_n_s16(0);
  ret.m128.val[1] = vdupq_n_s16(0);
#endif /* __ARM_NEON    */
#endif /* __SSE4_1__  */
#endif /* __AVX2__ */
  return ret;
}

#endif /* SRSRAN_SIMD_C16_SIZE */

#if SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE

inline simd_s_t srsran_simd_convert_2f_s(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  __m512 aa = _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b),
      b);
  __m512 bb = _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x04, 0x05, 0x06, 0x07, 0x0c, 0x0d, 0x0e, 0x0f, 0x14, 0x15, 0x16, 0x17, 0x1c, 0x1d, 0x1e, 0x1f),
      b);
  __m512i ai = _mm512_cvt_roundps_epi32(aa, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
  __m512i bi = _mm512_cvt_roundps_epi32(bb, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
  return _mm512_packs_epi32(ai, bi);
#else /* __AVX512F__ */
#ifdef __AVX2__
  __m256  aa = _mm256_round_ps(_mm256_permute2f128_ps(a, b, 0x20), _MM_FROUND_NINT);
  __m256  bb = _mm256_round_ps(_mm256_permute2f128_ps(a, b, 0x31), _MM_FROUND_NINT);
  __m256i ai = _mm256_cvtps_epi32(aa);
  __m256i bi = _mm256_cvtps_epi32(bb);
  return _mm256_packs_epi32(ai, bi);
#else
#ifdef __SSE4_1__
  __m128  aa      = _mm_round_ps(a, _MM_FROUND_NINT);
  __m128  bb      = _mm_round_ps(b, _MM_FROUND_NINT);
  __m128i ai      = _mm_cvtps_epi32(aa);
  __m128i bi      = _mm_cvtps_epi32(bb);
  return _mm_packs_epi32(ai, bi);
#else
#ifdef __ARM_NEON
  int32x4_t ai = vcvtnq_s32_f32(a);
  int32x4_t bi = vcvtnq_s32_f32(b);
  return (simd_s_t)vcombine_s16(vqmovn_s32(ai), vqmovn_s32(bi));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

// Converts 2 vectors of single-precision floats to a vector of int16_t, given that input vectors contain values of the
// interleaved data read from memory.
inline simd_s_t srsran_simd_convert_2f_interleaved_s(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  __m512  aa = _mm512_unpacklo_ps(a, b);
  __m512  bb = _mm512_unpackhi_ps(a, b);
  __m512i ai = _mm512_cvt_roundps_epi32(aa, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
  __m512i bi = _mm512_cvt_roundps_epi32(bb, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
  return _mm512_packs_epi32(ai, bi);
#else /* __AVX512F__ */
#ifdef __AVX2__
  __m256  aa = _mm256_round_ps(_mm256_unpacklo_ps(a, b), _MM_FROUND_NINT);
  __m256  bb = _mm256_round_ps(_mm256_unpackhi_ps(a, b), _MM_FROUND_NINT);
  __m256i ai = _mm256_cvtps_epi32(aa);
  __m256i bi = _mm256_cvtps_epi32(bb);
  return _mm256_packs_epi32(ai, bi);
#else
#ifdef __SSE4_1__
  __m128  aa = _mm_round_ps(_mm_unpacklo_ps(a, b), _MM_FROUND_NINT);
  __m128  bb = _mm_round_ps(_mm_unpackhi_ps(a, b), _MM_FROUND_NINT);
  __m128i ai = _mm_cvtps_epi32(aa);
  __m128i bi = _mm_cvtps_epi32(bb);
  return _mm_packs_epi32(ai, bi);
#else
#ifdef __ARM_NEON
  int32x4_t   ai                 = vcvtnq_s32_f32(a);
  int32x4_t   bi                 = vcvtnq_s32_f32(b);
  int16x4x2_t ab_s16_interleaved = vzip_s16(vqmovn_s32(ai), vqmovn_s32(bi));
  return (simd_s_t)vcombine_s16(ab_s16_interleaved.val[0], ab_s16_interleaved.val[1]);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

#ifdef __AVX512F__
inline simd_s_t srsran_simd_convert_1f_bf16(simd_f_t a)
{
  simd_s_t ret;

  const __m512i  bias      = _mm512_set1_epi32(0x7fff);
  const __m512i  one       = _mm512_set1_epi32(0x1);
  const __m512i  mask      = _mm512_set1_epi32(0xffff0000);
  const uint16_t zero_mask = 0x00ff;

  __m512i a_i32 = _mm512_castps_si512(a);

  // Round to nearest even.
  a_i32 = _mm512_add_epi32(a_i32, _mm512_add_epi32(bias, _mm512_and_si512(_mm512_srli_epi32(a_i32, 16), one)));

  // Input:            a0 xx | a1 xx | a2 xx | a3 xx | a4 xx | a5 xx | a6 xx | a7 xx | ... | a15 xx |
  // Transformations:
  //_mm512_srli_epi32  00 a0 | 00 a1 | 00 a2 | 00 a3 | 00 a4 | 00 a5 | 00 a6 | 00 a7 | ... | 00 a15 |
  //_mm512_and_si512   a0 00 | a1 00 | a2 00 | a3 00 | a4 00 | a5 00 | a6 00 | a7 00 | ... | a15 00 |
  // _mm512_maskz_permutex2var_epi32
  //                   a1 00 | a2 00 | a3 00 | a4 00 | a5 00 | a6 00 | a7 00 | a8 00 | a9 00 | a10 00 |  ...  | a0 00 |
  //_mm512_or_si512    a1 a0 | a2 a1 | a3 a2 | a4 a3 | a5 a4 | a6 a5 | a7 a6 | a8 a7 | a9 a8 | a10 a9 | a11 a10 | ... |

  // Remove the 16 least significant bits of the fractional part.
  __m512i tmp_a_lsb    = _mm512_srli_epi32(a_i32, 16);
  __m512i tmp_a_masked = _mm512_and_si512(a_i32, mask);

  __m512i tmp_a_msb = _mm512_maskz_permutex2var_epi32(
      0x7fff,
      tmp_a_masked,
      _mm512_setr_epi32(0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x0),
      tmp_a_masked);
  __m512i a_packed = _mm512_or_si512(tmp_a_lsb, tmp_a_msb);

  // Pack results into an output vector.
  ret = _mm512_maskz_permutex2var_epi32(
      zero_mask,
      a_packed,
      _mm512_setr_epi32(0x0, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x0, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe),
      a_packed);

  return ret;
}
#endif // __AVX512F__

static inline simd_s_t srsran_simd_convert_2f_bf16(simd_f_t a, simd_f_t b)
{
  simd_s_t ret;
#ifdef __AVX512F__
  const __m512i bias = _mm512_set1_epi32(0x7fff);
  const __m512i one  = _mm512_set1_epi32(0x1);

  __m512i a_i32 = _mm512_castps_si512(a);
  __m512i b_i32 = _mm512_castps_si512(b);

  // Round to nearest even.
  a_i32 = _mm512_add_epi32(a_i32, _mm512_add_epi32(bias, _mm512_and_si512(_mm512_srli_epi32(a_i32, 16), one)));
  b_i32 = _mm512_add_epi32(b_i32, _mm512_add_epi32(bias, _mm512_and_si512(_mm512_srli_epi32(b_i32, 16), one)));

#ifdef __AVX512BW__
  // Use 16 MSBs of each 32-bit input register and pack them in one resulting vector.
  ret = _mm512_permutex2var_epi16(a_i32,
                                  _mm512_setr_epi32(0x030001,
                                                    0x070005,
                                                    0x0b0009,
                                                    0x0f000d,
                                                    0x130011,
                                                    0x170015,
                                                    0x1b0019,
                                                    0x1f001d,
                                                    0x230021,
                                                    0x270025,
                                                    0x2b0029,
                                                    0x2f002d,
                                                    0x330031,
                                                    0x370035,
                                                    0x3b0039,
                                                    0x3f003d),
                                  b_i32);
#else  // __AVX512BW__
  const __m512i mask = _mm512_set1_epi32(0xffff0000);
  // Input:            a0 xx | a1 xx | a2 xx | a3 xx | a4 xx | a5 xx | a6 xx | a7 xx | ... | a15 xx |
  // Transformations:
  //_mm512_srli_epi32  00 a0 | 00 a1 | 00 a2 | 00 a3 | 00 a4 | 00 a5 | 00 a6 | 00 a7 | ... | 00 a15 |
  //_mm512_and_si512   a0 00 | a1 00 | a2 00 | a3 00 | a4 00 | a5 00 | a6 00 | a7 00 | ... | a15 00 |
  // _mm512_maskz_permutex2var_epi32
  //                   a1 00 | a2 00 | a3 00 | a4 00 | a5 00 | a6 00 | a7 00 | a8 00 | a9 00 | a10 00 |  ...  | a0 00 |
  //_mm512_or_si512    a1 a0 | a2 a1 | a3 a2 | a4 a3 | a5 a4 | a6 a5 | a7 a6 | a8 a7 | a9 a8 | a10 a9 | a11 a10 | ... |

  // Remove the 16 least significant bits of the fractional part.
  __m512i tmp_a_lsb    = _mm512_srli_epi32(a_i32, 16);
  __m512i tmp_a_masked = _mm512_and_si512(a_i32, mask);

  __m512i tmp_a_msb = _mm512_maskz_permutex2var_epi32(
      0x7fff,
      tmp_a_masked,
      _mm512_setr_epi32(0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x0),
      tmp_a_masked);
  __m512i a_packed = _mm512_or_si512(tmp_a_lsb, tmp_a_msb);

  // Remove the 16 least significant bits of the fractional part.
  __m512i tmp_b_lsb    = _mm512_srli_epi32(b_i32, 16);
  __m512i tmp_b_masked = _mm512_and_si512(b_i32, mask);

  __m512i tmp_b_msb = _mm512_maskz_permutex2var_epi32(
      0x7fff,
      tmp_b_masked,
      _mm512_setr_epi32(0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x0),
      tmp_b_masked);
  __m512i b_packed = _mm512_or_si512(tmp_b_lsb, tmp_b_msb);

  // Pack results into an output vector.
  ret = _mm512_permutex2var_epi32(
      a_packed,
      _mm512_setr_epi32(0x0, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e),
      b_packed);
#endif // __AVX512BW__
#else  /* __AVX512F__ */
#ifdef __AVX2__
  const __m256i bias = _mm256_set1_epi32(0x7fff);
  const __m256i one  = _mm256_set1_epi32(0x1);
  const __m256i mask = _mm256_set1_epi32(0xffff0000);

  __m256i a_i32 = _mm256_castps_si256(a);
  __m256i b_i32 = _mm256_castps_si256(b);

  // Round to nearest even.
  a_i32 = _mm256_add_epi32(a_i32, _mm256_add_epi32(bias, _mm256_and_si256(_mm256_srli_epi32(a_i32, 16), one)));
  b_i32 = _mm256_add_epi32(b_i32, _mm256_add_epi32(bias, _mm256_and_si256(_mm256_srli_epi32(b_i32, 16), one)));

  // Remove the 16 least significant bits of the fractional part.
  __m256i tmp_a_lsb    = _mm256_srli_epi32(a_i32, 16);
  __m256i tmp_a_masked = _mm256_and_si256(a_i32, mask);

  __m256i tmp_a_msb =
      _mm256_permutevar8x32_epi32(tmp_a_masked, _mm256_setr_epi32(0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x0));
  __m256i a_packed = _mm256_or_si256(tmp_a_lsb, tmp_a_msb);
  a_packed         = _mm256_permutevar8x32_epi32(a_packed, _mm256_setr_epi32(0x0, 0x2, 0x4, 0x6, 0x1, 0x3, 0x5, 0x7));

  // Remove the 16 least significant bits of the fractional part.
  __m256i tmp_b_lsb    = _mm256_srli_epi32(b_i32, 16);
  __m256i tmp_b_masked = _mm256_and_si256(b_i32, mask);

  __m256i tmp_b_msb =
      _mm256_permutevar8x32_epi32(tmp_b_masked, _mm256_setr_epi32(0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x0));
  __m256i b_packed = _mm256_or_si256(tmp_b_lsb, tmp_b_msb);
  b_packed         = _mm256_permutevar8x32_epi32(b_packed, _mm256_setr_epi32(0x1, 0x3, 0x5, 0x7, 0x0, 0x2, 0x4, 0x6));

  // Pack results into an output vector.
  ret                = _mm256_blend_epi32(a_packed, b_packed, 0b11110000);

#else /* __AVX2__ */
#ifdef __SSE4_1__
  const __m128i bias = _mm_set1_epi32(0x7fff);
  const __m128i one  = _mm_set1_epi32(0x1);
  const __m128i mask = _mm_set1_epi32(0xffff0000);

  __m128i a_i32 = _mm_castps_si128(a);
  __m128i b_i32 = _mm_castps_si128(b);

  // Round to nearest even.
  a_i32 = _mm_add_epi32(a_i32, _mm_add_epi32(bias, _mm_and_si128(_mm_srli_epi32(a_i32, 16), one)));
  b_i32 = _mm_add_epi32(b_i32, _mm_add_epi32(bias, _mm_and_si128(_mm_srli_epi32(b_i32, 16), one)));

  // Remove the 16 least significant bits of the fractional part.
  __m128i tmp_a_lsb    = _mm_srli_epi32(a_i32, 16);
  __m128i tmp_a_masked = _mm_and_si128(a_i32, mask);
  __m128i tmp_a_msb    = _mm_shuffle_epi32(tmp_a_masked, 0b00111001);
  __m128i a_packed     = _mm_or_si128(tmp_a_lsb, tmp_a_msb);
  a_packed             = _mm_shuffle_epi32(a_packed, 0b11001000);

  // Remove the 16 least significant bits of the fractional part.
  __m128i tmp_b_lsb    = _mm_srli_epi32(b_i32, 16);
  __m128i tmp_b_masked = _mm_and_si128(b_i32, mask);
  __m128i tmp_b_msb    = _mm_shuffle_epi32(tmp_b_masked, 0b00111001);
  __m128i b_packed     = _mm_or_si128(tmp_b_lsb, tmp_b_msb);
  b_packed             = _mm_shuffle_epi32(b_packed, 0b10001100);

  // Pack results into an output vector.
  ret                = _mm_blend_epi16(a_packed, b_packed, 0xf0);
#else /* __ARM_NEON */
#ifdef __ARM_NEON
#ifdef __ARM_FEATURE_BF16_VECTOR_ARITHMETIC
  bfloat16x4_t tmp1     = vcvt_bf16_f32(a);
  bfloat16x4_t tmp2     = vcvt_bf16_f32(b);
  ret                   = vreinterpretq_s16_bf16(vcombine_bf16(tmp1, tmp2));
#else  // __ARM_FEATURE_BF16_VECTOR_ARITHMETIC
  const uint32x4_t bias = vdupq_n_u32(0x7fff);
  const uint32x4_t one  = vdupq_n_u32(0x1);

  uint32x4_t a_i32 = vreinterpretq_u32_f32(a);
  uint32x4_t b_i32 = vreinterpretq_u32_f32(b);

  a_i32 = vaddq_u32(a_i32, vaddq_u32(bias, vandq_u32(vshrq_n_u32(a_i32, 16), one)));
  b_i32 = vaddq_u32(b_i32, vaddq_u32(bias, vandq_u32(vshrq_n_u32(b_i32, 16), one)));

  // Remove the 16 least significant bits of the fractional part.
  uint16x8_t tmp_a_1  = vreinterpretq_u16_u32(vshrq_n_u32(a_i32, 16));
  uint16x8_t tmp_a_2  = vextq_u16(tmp_a_1, tmp_a_1, 1);
  uint32x4_t a_packed = vreinterpretq_u32_u16(vorrq_u16(tmp_a_1, tmp_a_2));

  // Remove the 16 least significant bits of the fractional part.
  uint16x8_t tmp_b_1  = vreinterpretq_u16_u32(vshrq_n_u32(b_i32, 16));
  uint16x8_t tmp_b_2  = vextq_u16(tmp_b_1, tmp_b_1, 1);
  uint32x4_t b_packed = vreinterpretq_u32_u16(vorrq_u16(tmp_b_1, tmp_b_2));

  ret = vreinterpretq_s16_u32(vuzpq_u32(a_packed, b_packed).val[0]);
#endif /* __ARM_FEATURE_BF16_VECTOR_ARITHMETIC */
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
  return ret;
}

// Converts 2 vectors of single-precision floats to a vector of bf16_t, given that input vectors contain values of the
// interleaved data read from memory.
static inline simd_s_t srsran_simd_convert_2f_interleaved_bf16(simd_f_t a, simd_f_t b)
{
#ifdef __AVX512F__
  const __m512i bias = _mm512_set1_epi32(0x7fff);
  const __m512i one  = _mm512_set1_epi32(0x1);

  __m512i a_i32 = _mm512_castps_si512(a);
  __m512i b_i32 = _mm512_castps_si512(b);

  // Round to nearest even.
  a_i32 = _mm512_add_epi32(a_i32, _mm512_add_epi32(bias, _mm512_and_si512(_mm512_srli_epi32(a_i32, 16), one)));
  b_i32 = _mm512_add_epi32(b_i32, _mm512_add_epi32(bias, _mm512_and_si512(_mm512_srli_epi32(b_i32, 16), one)));

  // Pack both parts in 32-bit registers.
  return _mm512_or_si512(_mm512_and_si512(b_i32, _mm512_set1_epi32(0xffff0000)), _mm512_srli_epi32(a_i32, 16));
#else /* __AVX512F__ */
#ifdef __AVX2__
  const __m256i bias = _mm256_set1_epi32(0x7fff);
  const __m256i one  = _mm256_set1_epi32(0x1);

  __m256i a_i32 = _mm256_castps_si256(a);
  __m256i b_i32 = _mm256_castps_si256(b);

  // Round to nearest even.
  a_i32 = _mm256_add_epi32(a_i32, _mm256_add_epi32(bias, _mm256_and_si256(_mm256_srli_epi32(a_i32, 16), one)));
  b_i32 = _mm256_add_epi32(b_i32, _mm256_add_epi32(bias, _mm256_and_si256(_mm256_srli_epi32(b_i32, 16), one)));

  // Pack both parts in 32-bit registers.
  return _mm256_or_si256(_mm256_and_si256(b_i32, _mm256_set1_epi32(0xffff0000)), _mm256_srli_epi32(a_i32, 16));
#else /* __AVX512F__ */
#ifdef __SSE4_1__
  const __m128i bias = _mm_set1_epi32(0x7fff);
  const __m128i one  = _mm_set1_epi32(0x1);

  __m128i a_i32 = _mm_castps_si128(a);
  __m128i b_i32 = _mm_castps_si128(b);

  // Round to nearest even.
  a_i32 = _mm_add_epi32(a_i32, _mm_add_epi32(bias, _mm_and_si128(_mm_srli_epi32(a_i32, 16), one)));
  b_i32 = _mm_add_epi32(b_i32, _mm_add_epi32(bias, _mm_and_si128(_mm_srli_epi32(b_i32, 16), one)));

  // Pack both parts in 32-bit registers.
  return _mm_or_si128(_mm_and_si128(b_i32, _mm_set1_epi32(0xffff0000)), _mm_srli_epi32(a_i32, 16));
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  const uint32x4_t bias = vdupq_n_u32(0x7fff);
  const uint32x4_t one  = vdupq_n_u32(0x1);

  uint32x4_t a_i32 = vreinterpretq_u32_f32(a);
  uint32x4_t b_i32 = vreinterpretq_u32_f32(b);

  a_i32 = vaddq_u32(a_i32, vaddq_u32(bias, vandq_u32(vshrq_n_u32(a_i32, 16), one)));
  b_i32 = vaddq_u32(b_i32, vaddq_u32(bias, vandq_u32(vshrq_n_u32(b_i32, 16), one)));

  return vreinterpretq_s16_u32(vorrq_u32(vandq_u32(b_i32, vdupq_n_u32(0xffff0000)), vshrq_n_u32(a_i32, 16)));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_bf16_loadu(simd_f_t& even, simd_f_t& odd, const bf16_t* ptr)
{
#ifdef __AVX512F__
  // Load 32 16-bit brain float values.
  __m512i temp = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr));
  // Unpack brain floats from 32-bit registers.
  __m512i temp_even = _mm512_slli_epi32(temp, 16);
  __m512i temp_odd  = _mm512_and_si512(temp, _mm512_set1_epi32(0xffff0000));
  // Cast them to single-precision floating point numbers (doesn't generate any instruction).
  even = _mm512_castsi512_ps(temp_even);
  odd  = _mm512_castsi512_ps(temp_odd);
#else /* __AVX512F__ */
#ifdef __AVX2__
  __m256i temp = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
  even         = _mm256_castsi256_ps(_mm256_slli_epi32(temp, 16));
  odd          = _mm256_castsi256_ps(_mm256_and_si256(temp, _mm256_set1_epi32(0xffff0000)));
#else /* __AVX2__ */
#ifdef __SSE4_1__
  __m128i temp = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
  even         = _mm_castsi128_ps(_mm_slli_epi32(temp, 16));
  odd          = _mm_castsi128_ps(_mm_and_si128(temp, _mm_set1_epi32(0xffff0000)));
#else /* __ARM_NEON */
#ifdef __ARM_NEON
  uint32x4_t temp = vld1q_u32(reinterpret_cast<uint32_t const*>(ptr));
  even            = vreinterpretq_f32_u32(vshlq_n_u32(temp, 16));
  odd             = vreinterpretq_f32_u32(vandq_u32(temp, vdupq_n_u32(0xffff0000)));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

#ifdef SRSRAN_SIMD_CF_SIZE
inline simd_cf_t srsran_simd_cbf16_loadu(const cbf16_t* ptr)
{
  simd_cf_t ret;
#ifdef __ARM_NEON
  srsran_simd_bf16_loadu(ret.val[0], ret.val[1], reinterpret_cast<const bf16_t*>(ptr));
#else // __ARM_NEON
  srsran_simd_bf16_loadu(ret.re, ret.im, reinterpret_cast<const bf16_t*>(ptr));
#endif
  return ret;
}
#endif // SRSRAN_SIMD_CF_SIZE

inline void srsran_simd_bf16_storeu(bf16_t* ptr, simd_f_t a, simd_f_t b)
{
  simd_s_t bf16_vec = srsran_simd_convert_2f_bf16(a, b);
#ifdef __AVX512F__
  _mm512_storeu_si512(reinterpret_cast<__m512i*>(ptr), bf16_vec);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr), bf16_vec);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_storeu_si128(reinterpret_cast<__m128i*>(ptr), bf16_vec);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  vst1q_u32(reinterpret_cast<uint32_t*>(ptr), vreinterpretq_u32_s16(bf16_vec));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

#ifdef SRSRAN_SIMD_CF_SIZE
inline void srsran_simd_cbf16_storeu(cbf16_t* ptr, simd_cf_t simdreg)
{
  simd_s_t packed_iq_bf16 =
      srsran_simd_convert_2f_interleaved_bf16(srsran_simd_cf_re(simdreg), srsran_simd_cf_im(simdreg));

#ifdef __AVX512F__
  _mm512_storeu_si512(reinterpret_cast<__m512i*>(ptr), packed_iq_bf16);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr), packed_iq_bf16);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_storeu_si128(reinterpret_cast<__m128i*>(ptr), packed_iq_bf16);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  vst1q_u32(reinterpret_cast<uint32_t*>(ptr), vreinterpretq_u32_s16(packed_iq_bf16));
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}
#endif // SRSRAN_SIMD_CF_SIZE

#endif /* SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_C16_SIZE */

#if SRSRAN_SIMD_B_SIZE

///
/// Data types.
///

#ifdef __AVX512F__
using simd_b_t = __m512i;
#else /* __AVX512F__ */
#ifdef __AVX2__
using simd_b_t = __m256i;
#else /* __AVX2__ */
#ifdef __SSE4_1__
using simd_b_t = __m128i;
#else /* __ARM_NEON */
#ifdef __ARM_NEON
using simd_b_t = int8x16_t;
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */

inline simd_b_t srsran_simd_b_load(const int8_t* ptr)
{
#ifdef __AVX512F__
  return _mm512_load_si512(ptr);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_load_si256(reinterpret_cast<const __m256i*>(ptr));
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_load_si128(reinterpret_cast<const __m128i*>(ptr));
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vld1q_s8(ptr);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_b_t srsran_simd_b_loadu(const int8_t* ptr)
{
#ifdef __AVX512F__
  return _mm512_loadu_si512(ptr);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vld1q_s8(ptr);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_b_store(int8_t* ptr, simd_b_t simdreg)
{
#ifdef __AVX512F__
  _mm512_store_si512(ptr, simdreg);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_store_si256(reinterpret_cast<__m256i*>(ptr), simdreg);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_store_si128(reinterpret_cast<__m128i*>(ptr), simdreg);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  vst1q_s8(ptr, simdreg);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline void srsran_simd_b_storeu(int8_t* ptr, simd_b_t simdreg)
{
#ifdef __AVX512F__
  _mm512_storeu_si512(ptr, simdreg);
#else /* __AVX512F__ */
#ifdef __AVX2__
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(ptr), simdreg);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  _mm_storeu_si128(reinterpret_cast<__m128i*>(ptr), simdreg);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  vst1q_s8(ptr, simdreg);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_b_t srsran_simd_b_xor(simd_b_t a, simd_b_t b)
{
#ifdef __AVX512F__
  return _mm512_xor_epi32(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_xor_si256(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_xor_si128(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return veorq_s8(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_b_t srsran_simd_b_add(simd_b_t a, simd_b_t b)
{
#ifdef __AVX512F__
  return _mm512_adds_epi8(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_adds_epi8(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_adds_epi8(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vqaddq_s8(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_b_t srsran_simd_b_sub(simd_b_t a, simd_b_t b)
{
#ifdef __AVX512F__
  return _mm512_subs_epi8(a, b);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_subs_epi8(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_subs_epi8(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  return vqsubq_s8(a, b);
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

inline simd_b_t srsran_simd_b_neg(simd_b_t a, simd_b_t b)
{
#ifdef __AVX512F__
  __m256i a0 = _mm512_extracti64x4_epi64(a, 0);
  __m256i a1 = _mm512_extracti64x4_epi64(a, 1);
  __m256i b0 = _mm512_extracti64x4_epi64(b, 0);
  __m256i b1 = _mm512_extracti64x4_epi64(b, 1);
  __m256i r0 = _mm256_sign_epi8(a0, b0);
  __m256i r1 = _mm256_sign_epi8(a1, b1);
  return _mm512_inserti64x4(_mm512_broadcast_i64x4(r0), r1, 1);
#else /* __AVX512F__ */
#ifdef __AVX2__
  return _mm256_sign_epi8(a, b);
#else /* __AVX2__ */
#ifdef __SSE4_1__
  return _mm_sign_epi8(a, b);
#else /* __SSE4_1__ */
#ifdef __ARM_NEON
  // Taken and modified from sse2neon.h licensed under MIT.
  // Source: https://github.com/DLTcollab/sse2neon
  int8x16_t zero = vdupq_n_s8(0);
  // Signed shift right: faster than vclt.
  // (b < 0) ? 0xff : 0
  uint8x16_t ltMask = vreinterpretq_u8_s8(vshrq_n_s8(b, 7));
  // (b == 0) ? 0xff : 0
  int8x16_t zeroMask = vreinterpretq_s8_u8(vceqq_s8(b, zero));
  // -a
  int8x16_t neg = vnegq_s8(a);
  // Bitwise select either a or neg based on ltMask.
  int8x16_t masked = vbslq_s8(ltMask, neg, a);
  // res = masked & (~zeroMask)
  int8x16_t res = vbicq_s8(masked, zeroMask);
  return res;
#endif /* __ARM_NEON */
#endif /* __SSE4_1__ */
#endif /* __AVX2__ */
#endif /* __AVX512F__ */
}

#endif /* SRSRAN_SIMD_B_SIZE */

} // namespace srsran
