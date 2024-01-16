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

#pragma once

#include "srsran/adt/complex.h"

// Enables intel intrinsics, it includes AVX, AVX2, FMA, AVX512.
#ifdef HAVE_SSE

// Enables optimizations in this file.
#ifndef __OPTIMIZE__
#define __OPTIMIZE__
#endif // __OPTIMIZE__

// gcc-12 gives a likely false alarm when including AVX512 intrinsics. Disable maybe-uninitialized diagnostics.
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif // __clang__

#include <immintrin.h>

#ifndef __clang__
#pragma GCC diagnostic pop
#endif // __clang__
#endif // HAVE_SSE

#ifdef HAVE_NEON
#include <arm_neon.h>
#endif // HAVE_NEON

namespace srsran {

inline bool is_simd_addr_aligned(const void* addr, uintptr_t mask)
{
  uintptr_t addr_i = reinterpret_cast<uintptr_t>(addr);
  return (addr_i & mask) == 0;
}

/*
 * SIMD Vector bit alignment
 */
#ifdef HAVE_AVX512
#define SIMD_BYTE_ALIGN 64
#define SIMD_IS_ALIGNED(PTR) is_simd_addr_aligned(PTR, 0x3f)

#else /* HAVE_AVX512 */
#ifdef HAVE_AVX
#define SIMD_BYTE_ALIGN 32
#define SIMD_IS_ALIGNED(PTR) is_simd_addr_aligned(PTR, 0x1f)
#else /* HAVE_AVX */
#ifdef HAVE_SSE
#define SIMD_BYTE_ALIGN 16
#define SIMD_IS_ALIGNED(PTR) is_simd_addr_aligned(PTR, 0x0f)
#else /* HAVE_SSE */
#define SIMD_BYTE_ALIGN 16
#define SIMD_IS_ALIGNED(PTR) (true)
#endif /* HAVE_SSE */
#endif /* HAVE_AVX */
#endif /* HAVE_AVX512 */

/* Memory Sizes for Single Floating Point and fixed point */
#ifdef HAVE_AVX512

#define SRSRAN_SIMD_F_SIZE 16
#define SRSRAN_SIMD_CF_SIZE 16

#define SRSRAN_SIMD_I_SIZE 16

#define SRSRAN_SIMD_B_SIZE 64
#define SRSRAN_SIMD_S_SIZE 32
#define SRSRAN_SIMD_C16_SIZE 0

#else
#ifdef HAVE_AVX2

#define SRSRAN_SIMD_F_SIZE 8
#define SRSRAN_SIMD_CF_SIZE 8

#define SRSRAN_SIMD_I_SIZE 8

#define SRSRAN_SIMD_B_SIZE 32
#define SRSRAN_SIMD_S_SIZE 16
#define SRSRAN_SIMD_C16_SIZE 16

#else /* HAVE_AVX2 */
#ifdef HAVE_SSE

#define SRSRAN_SIMD_F_SIZE 4
#define SRSRAN_SIMD_CF_SIZE 4

#define SRSRAN_SIMD_I_SIZE 4

#define SRSRAN_SIMD_B_SIZE 16
#define SRSRAN_SIMD_S_SIZE 8
#define SRSRAN_SIMD_C16_SIZE 8

#else /* HAVE_SSE */
#ifdef HAVE_NEON

#define SRSRAN_SIMD_F_SIZE 4
#define SRSRAN_SIMD_CF_SIZE 4

#define SRSRAN_SIMD_I_SIZE 4
#define SRSRAN_SIMD_B_SIZE 16
#define SRSRAN_SIMD_S_SIZE 8
#define SRSRAN_SIMD_C16_SIZE 8

#else /* HAVE_NEON */
#define SRSRAN_SIMD_F_SIZE 0
#define SRSRAN_SIMD_CF_SIZE 0

#define SRSRAN_SIMD_I_SIZE 0
#define SRSRAN_SIMD_B_SIZE 0
#define SRSRAN_SIMD_S_SIZE 0
#define SRSRAN_SIMD_C16_SIZE 0

#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */

#ifndef ENABLE_C16
#undef SRSRAN_SIMD_C16_SIZE
#define SRSRAN_SIMD_C16_SIZE 0
#endif /* ENABLE_C16 */

#if SRSRAN_SIMD_F_SIZE

/* Data types */
#ifdef HAVE_AVX512
typedef __m512 simd_f_t;
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
typedef __m256 simd_f_t;
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
typedef __m128 simd_f_t;
#else /* HAVE_NEON */
#ifdef HAVE_NEON
typedef float32x4_t simd_f_t;
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */

/* Single precision Floating point functions */
static inline simd_f_t srsran_simd_f_load(const float* ptr)
{
#ifdef HAVE_AVX512
  return _mm512_load_ps(ptr);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_load_ps(ptr);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_load_ps(ptr);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_f32(ptr);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_loadu(const float* ptr)
{
#ifdef HAVE_AVX512
  return _mm512_loadu_ps(ptr);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_loadu_ps(ptr);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_loadu_ps(ptr);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_f32(ptr);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_f_store(float* ptr, simd_f_t simdreg)
{
#ifdef HAVE_AVX512
  _mm512_store_ps(ptr, simdreg);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  _mm256_store_ps(ptr, simdreg);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  _mm_store_ps(ptr, simdreg);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_f32(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_f_storeu(float* ptr, simd_f_t simdreg)
{
#ifdef HAVE_AVX512
  _mm512_storeu_ps(ptr, simdreg);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  _mm256_storeu_ps(ptr, simdreg);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  _mm_storeu_ps(ptr, simdreg);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_f32(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_set1(float x)
{
#ifdef HAVE_AVX512
  return _mm512_set1_ps(x);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_set1_ps(x);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_set1_ps(x);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vdupq_n_f32(x);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_mul(simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
  return _mm512_mul_ps(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_mul_ps(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_mul_ps(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vmulq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_rcp(simd_f_t a)
{
#ifdef HAVE_AVX512
  return _mm512_rcp14_ps(a);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_rcp_ps(a);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_rcp_ps(a);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vmulq_f32(vrecpeq_f32(a), vrecpsq_f32(vrecpeq_f32(a), a));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_addsub(simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
  __m512 r = _mm512_add_ps(a, b);
  return _mm512_mask_sub_ps(r, 0b0101010101010101, a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_addsub_ps(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_addsub_ps(a, b);
#else            /* HAVE_SSE */
#ifdef HAVE_NEON // CURRENTLY USES GENERIC IMPLEMENTATION FOR NEON
  float*   a_ptr = (float*)&a;
  float*   b_ptr = (float*)&b;
  simd_f_t ret;
  float*   c_ptr = (float*)&ret;
  for (int i = 0; i < 4; i++) {
    if (i % 2 == 0) {
      c_ptr[i] = a_ptr[i] - b_ptr[i];
    } else {
      c_ptr[i] = a_ptr[i] + b_ptr[i];
    }
  }

  return ret;
#endif           /* HAVE_NEON */
#endif           /* HAVE_SSE */
#endif           /* HAVE_AVX2 */
#endif           /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_sub(simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
  return _mm512_sub_ps(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_sub_ps(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_sub_ps(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vsubq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_add(simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
  return _mm512_add_ps(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_add_ps(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_add_ps(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vaddq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_fma(simd_f_t acc, simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
  return _mm512_fmadd_ps(a, b, acc);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_fmadd_ps(a, b, acc);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_fmadd_ps(a, b, acc);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vmlaq_f32(acc, a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_zero(void)
{
#ifdef HAVE_AVX512
  return _mm512_setzero_ps();
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_setzero_ps();
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_setzero_ps();
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vdupq_n_f32(0);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_swap(simd_f_t a)
{
#ifdef HAVE_AVX512
  return _mm512_permute_ps(a, 0b10110001);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_permute_ps(a, 0b10110001);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_shuffle_ps(a, a, 0b10110001);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vcombine_f32(vrev64_f32(vget_low_f32(a)), vrev64_f32(vget_high_f32(a)));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_hadd(simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
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
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  simd_f_t a1 = _mm256_permute2f128_ps(a, b, 0b00100000);
  simd_f_t b1 = _mm256_permute2f128_ps(a, b, 0b00110001);
  return _mm256_hadd_ps(a1, b1);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_hadd_ps(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vcombine_f32(vpadd_f32(vget_low_f32(a), vget_high_f32(a)), vpadd_f32(vget_low_f32(b), vget_high_f32(b)));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_sqrt(simd_f_t a)
{
#ifdef HAVE_AVX512
  return _mm512_sqrt_ps(a);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_sqrt_ps(a);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_sqrt_ps(a);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  float32x4_t sqrt_reciprocal = vrsqrteq_f32(a);
  sqrt_reciprocal    = vmulq_f32(vrsqrtsq_f32(vmulq_f32(a, sqrt_reciprocal), sqrt_reciprocal), sqrt_reciprocal);
  float32x4_t result = vmulq_f32(a, sqrt_reciprocal);

  /* Detect zeros in NEON 1/sqrtf for preventing NaN */
  float32x4_t zeros = vmovq_n_f32(0);      /* Zero vector */
  uint32x4_t  mask  = vceqq_f32(a, zeros); /* Zero vector mask */
  return vbslq_f32(mask, zeros, result);   /* Force zero results and return */
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_neg(simd_f_t a)
{
#ifdef HAVE_AVX512
  return _mm512_xor_ps(_mm512_set1_ps(-0.0f), a);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_xor_ps(_mm256_set1_ps(-0.0f), a);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_xor_ps(_mm_set1_ps(-0.0f), a);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vnegq_f32(a);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_neg_mask(simd_f_t a, simd_f_t mask)
{
#ifdef HAVE_AVX512
  return _mm512_xor_ps(mask, a);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_xor_ps(mask, a);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_xor_ps(mask, a);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return (float32x4_t)veorq_s32((int32x4_t)a, (int32x4_t)mask);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_abs(simd_f_t a)
{
#ifdef HAVE_AVX512
  return _mm512_andnot_ps(_mm512_set1_ps(-0.0f), a);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_andnot_ps(_mm256_set1_ps(-0.0f), a);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_andnot_ps(_mm_set1_ps(-0.0f), a);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vabsq_f32(a);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_f_fprintf(FILE* stream, simd_f_t a)
{
  float x[SRSRAN_SIMD_F_SIZE];

  srsran_simd_f_storeu(x, a);

  fprintf(stream, "[");
  for (int i = 0; i < SRSRAN_SIMD_F_SIZE; i++) {
    fprintf(stream, "%+2.5f, ", x[i]);
  }
  fprintf(stream, "];\n");
}

#endif /* SRSRAN_SIMD_F_SIZE */

#if SRSRAN_SIMD_CF_SIZE

#ifdef HAVE_NEON
typedef float32x4x2_t simd_cf_t;
#else
typedef struct {
  simd_f_t re;
  simd_f_t im;
} simd_cf_t;
#endif

/* Complex Single precission Floating point functions */
static inline simd_cf_t srsran_simd_cfi_load(const cf_t* ptr)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  __m512 in1 = _mm512_load_ps((float*)(ptr));
  __m512 in2 = _mm512_load_ps((float*)(ptr + SRSRAN_SIMD_CF_SIZE / 2));
  ret.re     = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e),
      in2);
  ret.im = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f),
      in2);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  __m256 in1  = _mm256_permute_ps(_mm256_load_ps((float*)(ptr)), 0b11011000);
  __m256 in2  = _mm256_permute_ps(_mm256_load_ps((float*)(ptr + 4)), 0b11011000);
  ret.re      = _mm256_unpacklo_ps(in1, in2);
  ret.im      = _mm256_unpackhi_ps(in1, in2);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  __m128 i1 = _mm_load_ps((float*)(ptr));
  __m128 i2 = _mm_load_ps((float*)(ptr + 2));
  ret.re    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(2, 0, 2, 0));
  ret.im    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(3, 1, 3, 1));
#else
#ifdef HAVE_NEON
  ret        = vld2q_f32((float*)(ptr));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

/* Complex Single precission Floating point functions */
static inline simd_cf_t srsran_simd_cfi_loadu(const cf_t* ptr)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  __m512 in1 = _mm512_loadu_ps((float*)(ptr));
  __m512 in2 = _mm512_loadu_ps((float*)(ptr + SRSRAN_SIMD_CF_SIZE / 2));
  ret.re     = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e),
      in2);
  ret.im = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f),
      in2);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  __m256 in1  = _mm256_permute_ps(_mm256_loadu_ps((float*)(ptr)), 0b11011000);
  __m256 in2  = _mm256_permute_ps(_mm256_loadu_ps((float*)(ptr + 4)), 0b11011000);
  ret.re      = _mm256_unpacklo_ps(in1, in2);
  ret.im      = _mm256_unpackhi_ps(in1, in2);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  __m128 i1 = _mm_loadu_ps((float*)(ptr));
  __m128 i2 = _mm_loadu_ps((float*)(ptr + 2));
  ret.re    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(2, 0, 2, 0));
  ret.im    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(3, 1, 3, 1));
#else
#ifdef HAVE_NEON
  ret        = vld2q_f32((float*)(ptr));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_load(const float* re, const float* im)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_load_ps(re);
  ret.im = _mm512_load_ps(im);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  ret.re      = _mm256_load_ps(re);
  ret.im      = _mm256_load_ps(im);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  ret.re    = _mm_load_ps(re);
  ret.im    = _mm_load_ps(im);
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  ret.val[0] = vld1q_f32(re);
  ret.val[1] = vld1q_f32(im);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_loadu(const float* re, const float* im)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_loadu_ps(re);
  ret.im = _mm512_loadu_ps(im);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  ret.re      = _mm256_loadu_ps(re);
  ret.im      = _mm256_loadu_ps(im);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  ret.re    = _mm_loadu_ps(re);
  ret.im    = _mm_loadu_ps(im);
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  ret.val[0] = vld1q_f32(re);
  ret.val[1] = vld1q_f32(im);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline void srsran_simd_cfi_store(cf_t* ptr, simd_cf_t simdreg)
{
#ifdef HAVE_AVX512
  __m512 s1 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      simdreg.im);
  __m512 s2 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0a, 0x1a, 0x0b, 0x1b, 0x0c, 0x1c, 0x0d, 0x1d, 0x0e, 0x1e, 0x0f, 0x1f),
      simdreg.im);
  _mm512_store_ps((float*)(ptr), s1);
  _mm512_store_ps((float*)(ptr + 8), s2);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  __m256 out1 = _mm256_permute_ps(simdreg.re, 0b11011000);
  __m256 out2 = _mm256_permute_ps(simdreg.im, 0b11011000);
  _mm256_store_ps((float*)(ptr), _mm256_unpacklo_ps(out1, out2));
  _mm256_store_ps((float*)(ptr + 4), _mm256_unpackhi_ps(out1, out2));
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  _mm_store_ps((float*)(ptr), _mm_unpacklo_ps(simdreg.re, simdreg.im));
  _mm_store_ps((float*)(ptr + 2), _mm_unpackhi_ps(simdreg.re, simdreg.im));
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst2q_f32((float*)(ptr), simdreg);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_cfi_storeu(cf_t* ptr, simd_cf_t simdreg)
{
#ifdef HAVE_AVX512
  __m512 s1 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      simdreg.im);
  __m512 s2 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0a, 0x1a, 0x0b, 0x1b, 0x0c, 0x1c, 0x0d, 0x1d, 0x0e, 0x1e, 0x0f, 0x1f),
      simdreg.im);
  _mm512_storeu_ps((float*)(ptr), s1);
  _mm512_storeu_ps((float*)(ptr + 8), s2);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  __m256 out1 = _mm256_permute_ps(simdreg.re, 0b11011000);
  __m256 out2 = _mm256_permute_ps(simdreg.im, 0b11011000);
  _mm256_storeu_ps((float*)(ptr), _mm256_unpacklo_ps(out1, out2));
  _mm256_storeu_ps((float*)(ptr + 4), _mm256_unpackhi_ps(out1, out2));
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  _mm_storeu_ps((float*)(ptr), _mm_unpacklo_ps(simdreg.re, simdreg.im));
  _mm_storeu_ps((float*)(ptr + 2), _mm_unpackhi_ps(simdreg.re, simdreg.im));
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst2q_f32((float*)(ptr), simdreg);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_cf_store(float* re, float* im, simd_cf_t simdreg)
{
#ifdef HAVE_AVX512
  _mm512_store_ps(re, simdreg.re);
  _mm512_store_ps(im, simdreg.im);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  _mm256_store_ps(re, simdreg.re);
  _mm256_store_ps(im, simdreg.im);
#else /* HAVE_AVX512 */
#ifdef HAVE_SSE
  _mm_store_ps((float*)re, simdreg.re);
  _mm_store_ps((float*)im, simdreg.im);
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst1q_f32((float*)re, simdreg.val[0]);
  vst1q_f32((float*)im, simdreg.val[1]);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_cf_storeu(float* re, float* im, simd_cf_t simdreg)
{
#ifdef HAVE_AVX512
  _mm512_storeu_ps(re, simdreg.re);
  _mm512_storeu_ps(im, simdreg.im);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  _mm256_storeu_ps(re, simdreg.re);
  _mm256_storeu_ps(im, simdreg.im);
#else /* HAVE_AVX512 */
#ifdef HAVE_SSE
  _mm_storeu_ps((float*)re, simdreg.re);
  _mm_storeu_ps((float*)im, simdreg.im);
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst1q_f32((float*)re, simdreg.val[0]);
  vst1q_f32((float*)im, simdreg.val[1]);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_cf_re(simd_cf_t in)
{
#ifdef HAVE_NEON
  simd_f_t out = in.val[0];
#else
  simd_f_t out      = in.re;
#endif /*HAVE_NEON*/
#ifndef HAVE_AVX512
#ifdef HAVE_AVX2
  /* Permute for AVX registers (mis SSE registers) */
  const __m256i idx = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  out               = _mm256_permutevar8x32_ps(out, idx);
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return out;
}

static inline simd_f_t srsran_simd_cf_im(simd_cf_t in)
{
#ifdef HAVE_NEON
  simd_f_t out = in.val[1];
#else
  simd_f_t out      = in.im;
#endif /*HAVE_NEON*/
#ifndef HAVE_AVX512
#ifdef HAVE_AVX2
  /* Permute for AVX registers (mis SSE registers) */
  const __m256i idx = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  out               = _mm256_permutevar8x32_ps(out, idx);
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return out;
}

static inline simd_cf_t srsran_simd_cf_set1(cf_t x)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_set1_ps(x.real());
  ret.im = _mm512_set1_ps(x.imag());
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  ret.re            = _mm256_set1_ps(x.real());
  ret.im            = _mm256_set1_ps(x.imag());
#else
#ifdef HAVE_SSE
  ret.re            = _mm_set1_ps(x.real());
  ret.im            = _mm_set1_ps(x.imag());
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  ret.val[0]        = vdupq_n_f32(x.real());
  ret.val[1]        = vdupq_n_f32(x.imag());
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_prod(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_sub_ps(_mm512_mul_ps(a.re, b.re), _mm512_mul_ps(a.im, b.im));
  ret.im = _mm512_add_ps(_mm512_mul_ps(a.re, b.im), _mm512_mul_ps(a.im, b.re));
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
#ifdef HAVE_FMA
  ret.re            = _mm256_fmsub_ps(a.re, b.re, _mm256_mul_ps(a.im, b.im));
  ret.im            = _mm256_fmadd_ps(a.re, b.im, _mm256_mul_ps(a.im, b.re));
#else  /* HAVE_FMA */
  ret.re = _mm256_sub_ps(_mm256_mul_ps(a.re, b.re), _mm256_mul_ps(a.im, b.im));
  ret.im = _mm256_add_ps(_mm256_mul_ps(a.re, b.im), _mm256_mul_ps(a.im, b.re));
#endif /* HAVE_FMA */
#else
#ifdef HAVE_SSE
  ret.re            = _mm_sub_ps(_mm_mul_ps(a.re, b.re), _mm_mul_ps(a.im, b.im));
  ret.im            = _mm_add_ps(_mm_mul_ps(a.re, b.im), _mm_mul_ps(a.im, b.re));
#else
#ifdef HAVE_NEON
  ret.val[0]        = vsubq_f32(vmulq_f32(a.val[0], b.val[0]), vmulq_f32(a.val[1], b.val[1]));
  ret.val[1]        = vaddq_f32(vmulq_f32(a.val[0], b.val[1]), vmulq_f32(a.val[1], b.val[0]));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_conjprod(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_add_ps(_mm512_mul_ps(a.re, b.re), _mm512_mul_ps(a.im, b.im));
  ret.im = _mm512_sub_ps(_mm512_mul_ps(a.im, b.re), _mm512_mul_ps(a.re, b.im));
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  ret.re            = _mm256_add_ps(_mm256_mul_ps(a.re, b.re), _mm256_mul_ps(a.im, b.im));
  ret.im            = _mm256_sub_ps(_mm256_mul_ps(a.im, b.re), _mm256_mul_ps(a.re, b.im));
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  ret.re            = _mm_add_ps(_mm_mul_ps(a.re, b.re), _mm_mul_ps(a.im, b.im));
  ret.im            = _mm_sub_ps(_mm_mul_ps(a.im, b.re), _mm_mul_ps(a.re, b.im));
#else
#ifdef HAVE_NEON
  ret.val[0]        = vaddq_f32(vmulq_f32(a.val[0], b.val[0]), vmulq_f32(a.val[1], b.val[1]));
  ret.val[1]        = vsubq_f32(vmulq_f32(a.val[1], b.val[0]), vmulq_f32(a.val[0], b.val[1]));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_add(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_add_ps(a.re, b.re);
  ret.im = _mm512_add_ps(a.im, b.im);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  ret.re            = _mm256_add_ps(a.re, b.re);
  ret.im            = _mm256_add_ps(a.im, b.im);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  ret.re            = _mm_add_ps(a.re, b.re);
  ret.im            = _mm_add_ps(a.im, b.im);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  ret.val[0]        = vaddq_f32(a.val[0], b.val[0]);
  ret.val[1]        = vaddq_f32(a.val[1], b.val[1]);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_sub(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_sub_ps(a.re, b.re);
  ret.im = _mm512_sub_ps(a.im, b.im);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  ret.re            = _mm256_sub_ps(a.re, b.re);
  ret.im            = _mm256_sub_ps(a.im, b.im);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  ret.re            = _mm_sub_ps(a.re, b.re);
  ret.im            = _mm_sub_ps(a.im, b.im);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  ret.val[0]        = vsubq_f32(a.val[0], b.val[0]);
  ret.val[1]        = vsubq_f32(a.val[1], b.val[1]);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_f_t srsran_simd_cf_norm_sq(simd_cf_t a)
{
  return srsran_simd_f_fma(
      srsran_simd_f_mul(srsran_simd_cf_re(a), srsran_simd_cf_re(a)), srsran_simd_cf_im(a), srsran_simd_cf_im(a));
}

static inline simd_cf_t srsran_simd_cf_mul(simd_cf_t a, simd_f_t b)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_mul_ps(a.re, b);
  ret.im = _mm512_mul_ps(a.im, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  b                 = _mm256_permutevar8x32_ps(b, _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7));
  ret.re            = _mm256_mul_ps(a.re, b);
  ret.im            = _mm256_mul_ps(a.im, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  ret.re            = _mm_mul_ps(a.re, b);
  ret.im            = _mm_mul_ps(a.im, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  ret.val[0]        = vmulq_f32(a.val[0], b);
  ret.val[1]        = vmulq_f32(a.val[1], b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_rcp(simd_cf_t a)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  simd_f_t a2re     = _mm512_mul_ps(a.re, a.re);
  simd_f_t a2im     = _mm512_mul_ps(a.im, a.im);
  simd_f_t mod2     = _mm512_add_ps(a2re, a2im);
  simd_f_t rcp      = _mm512_rcp14_ps(mod2);
  simd_f_t neg_a_im = _mm512_xor_ps(_mm512_set1_ps(-0.0f), a.im);
  ret.re            = _mm512_mul_ps(a.re, rcp);
  ret.im            = _mm512_mul_ps(neg_a_im, rcp);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  simd_f_t a2re     = _mm256_mul_ps(a.re, a.re);
  simd_f_t a2im     = _mm256_mul_ps(a.im, a.im);
  simd_f_t mod2     = _mm256_add_ps(a2re, a2im);
  simd_f_t rcp      = _mm256_rcp_ps(mod2);
  simd_f_t neg_a_im = _mm256_xor_ps(_mm256_set1_ps(-0.0f), a.im);
  ret.re            = _mm256_mul_ps(a.re, rcp);
  ret.im            = _mm256_mul_ps(neg_a_im, rcp);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  simd_f_t a2re     = _mm_mul_ps(a.re, a.re);
  simd_f_t a2im     = _mm_mul_ps(a.im, a.im);
  simd_f_t mod2     = _mm_add_ps(a2re, a2im);
  simd_f_t rcp      = _mm_rcp_ps(mod2);
  simd_f_t neg_a_im = _mm_xor_ps(_mm_set1_ps(-0.0f), a.im);
  ret.re            = _mm_mul_ps(a.re, rcp);
  ret.im            = _mm_mul_ps(neg_a_im, rcp);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  simd_f_t a2re     = vmulq_f32(a.val[0], a.val[0]);
  simd_f_t a2im     = vmulq_f32(a.val[1], a.val[1]);
  simd_f_t mod2     = vaddq_f32(a2re, a2im);
  simd_f_t rcp      = vmulq_f32(vrecpeq_f32(mod2), vrecpsq_f32(vrecpeq_f32(mod2), mod2));
  simd_f_t neg_a_im = vnegq_f32(a.val[1]);
  ret.val[0]        = vmulq_f32(a.val[0], rcp);
  ret.val[1]        = vmulq_f32(neg_a_im, rcp);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_neg(simd_cf_t a)
{
  simd_cf_t ret;
#if HAVE_NEON
  ret.val[0] = srsran_simd_f_neg(a.val[0]);
  ret.val[1] = srsran_simd_f_neg(a.val[1]);
#else  /* HAVE_NEON */
  ret.re            = srsran_simd_f_neg(a.re);
  ret.im            = srsran_simd_f_neg(a.im);
#endif /* HAVE_NEON */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_neg_mask(simd_cf_t a, simd_f_t mask)
{
  simd_cf_t ret;
#ifndef HAVE_AVX512
#ifdef HAVE_AVX2
  mask = _mm256_permutevar8x32_ps(mask, _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7));
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
#if HAVE_NEON
  ret.val[0] = srsran_simd_f_neg_mask(a.val[0], mask);
  ret.val[1] = srsran_simd_f_neg_mask(a.val[1], mask);
#else  /* HAVE_NEON */
  ret.re            = srsran_simd_f_neg_mask(a.re, mask);
  ret.im            = srsran_simd_f_neg_mask(a.im, mask);
#endif /* HAVE_NEON */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_conj(simd_cf_t a)
{
  simd_cf_t ret;
#if HAVE_NEON
  ret.val[0] = a.val[0];
  ret.val[1] = srsran_simd_f_neg(a.val[1]);
#else  /* HAVE_NEON */
  ret.re            = a.re;
  ret.im            = srsran_simd_f_neg(a.im);
#endif /* HAVE_NEON */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_mulj(simd_cf_t a)
{
  simd_cf_t ret;
#if HAVE_NEON
  ret.val[0] = srsran_simd_f_neg(a.val[1]);
  ret.val[1] = a.val[0];
#else  /* HAVE_NEON */
  ret.re            = srsran_simd_f_neg(a.im);
  ret.im            = a.re;
#endif /* HAVE_NEON */
  return ret;
}

static inline simd_cf_t srsran_simd_cf_zero(void)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_setzero_ps();
  ret.im = _mm512_setzero_ps();
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  ret.re            = _mm256_setzero_ps();
  ret.im            = _mm256_setzero_ps();
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  ret.re            = _mm_setzero_ps();
  ret.im            = _mm_setzero_ps();
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  ret.val[0]        = vdupq_n_f32(0);
  ret.val[1]        = vdupq_n_f32(0);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline void srsran_simd_cf_fprintf(FILE* stream, simd_cf_t a)
{
  cf_t x[SRSRAN_SIMD_CF_SIZE];

  srsran_simd_cfi_storeu(x, a);

  fprintf(stream, "[");
  for (int i = 0; i < SRSRAN_SIMD_CF_SIZE; i++) {
    fprintf(stream, "%+2.5f%+2.5fi, ", x[i].real(), x[i].imag());
  }
  fprintf(stream, "];\n");
}

#endif /* SRSRAN_SIMD_CF_SIZE */

#if SRSRAN_SIMD_I_SIZE

#ifdef HAVE_AVX512
typedef __m512i   simd_i_t;
typedef __mmask16 simd_sel_t;
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
typedef __m256i simd_i_t;
typedef __m256  simd_sel_t;
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
typedef __m128i simd_i_t;
typedef __m128  simd_sel_t;
#else /* HAVE_AVX2 */
#ifdef HAVE_NEON
typedef int32x4_t  simd_i_t;
typedef uint32x4_t simd_sel_t;
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */

static inline simd_i_t srsran_simd_i_load(int32_t* x)
{
#ifdef HAVE_AVX512
  return _mm512_load_epi32((__m512i*)x);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_load_si256((__m256i*)x);
#else
#ifdef HAVE_SSE
  return _mm_load_si128((__m128i*)x);
#else
#ifdef HAVE_NEON
  return vld1q_s32((int*)x);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_i_store(int32_t* x, simd_i_t reg)
{
#ifdef HAVE_AVX512
  _mm512_store_epi32((__m512i*)x, reg);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  _mm256_store_si256((__m256i*)x, reg);
#else
#ifdef HAVE_SSE
  _mm_store_si128((__m128i*)x, reg);
#else
#ifdef HAVE_NEON
  vst1q_s32((int*)x, reg);
#endif /*HAVE_NEON*/
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_i_t srsran_simd_i_set1(int x)
{
#ifdef HAVE_AVX512
  return _mm512_set1_epi32(x);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_set1_epi32(x);
#else
#ifdef HAVE_SSE
  return _mm_set1_epi32(x);
#else
#ifdef HAVE_NEON
  return vdupq_n_s32(x);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_i_t srsran_simd_i_add(simd_i_t a, simd_i_t b)
{
#ifdef HAVE_AVX512
  return _mm512_add_epi32(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_add_epi32(a, b);
#else
#ifdef HAVE_SSE
  return _mm_add_epi32(a, b);
#else
#ifdef HAVE_NEON
  return vaddq_s32(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_i_t srsran_simd_i_mul(simd_i_t a, simd_i_t b)
{
#ifdef HAVE_AVX512
  return _mm512_mullo_epi32(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_mullo_epi32(a, b);
#else
#ifdef HAVE_SSE
  return _mm_mul_epi32(a, b);
#else
#ifdef HAVE_NEON
  return vmulq_s32(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_i_t srsran_simd_i_and(simd_i_t a, simd_i_t b)
{
#ifdef HAVE_AVX512
  return _mm512_and_si512(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_and_si256(a, b);
#else
#ifdef HAVE_SSE
  return _mm_and_si128(a, b);
#else
#ifdef HAVE_NEON
  return vandq_s32(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_sel_t srsran_simd_sel_and(simd_sel_t a, simd_sel_t b)
{
#ifdef HAVE_AVX512
  return _kand_mask16(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_and_ps(a, b);
#else
#ifdef HAVE_SSE
  return _mm_and_ps(a, b);
#else
#ifdef HAVE_NEON
  return vandq_u32(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_sel_t srsran_simd_f_max(simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
  return _mm512_cmp_ps_mask(a, b, _CMP_GT_OS);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_cmp_ps(a, b, _CMP_GT_OS);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return (simd_sel_t)_mm_cmpgt_ps(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vcgtq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_sel_t srsran_simd_f_min(simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
  return _mm512_cmp_ps_mask(a, b, _CMP_LT_OS);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_cmp_ps(a, b, _CMP_LT_OS);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return (simd_sel_t)_mm_cmplt_ps(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vcltq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_f_t srsran_simd_f_select(simd_f_t a, simd_f_t b, simd_sel_t selector)
{
#ifdef HAVE_AVX512
  return _mm512_mask_blend_ps(selector, (__m512)a, (__m512)b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_blendv_ps(a, b, selector);
#else
#ifdef HAVE_SSE
  return _mm_blendv_ps(a, b, selector);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vbslq_f32(selector, b, a);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_cf_t srsran_simd_cf_select(simd_cf_t a, simd_cf_t b, simd_sel_t selector)
{
  simd_cf_t ret;
#ifdef HAVE_AVX512
  ret.re = _mm512_mask_blend_ps(selector, a.re, b.re);
  ret.im = _mm512_mask_blend_ps(selector, a.im, b.im);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  /* Permute for AVX registers (mis SSE registers) */
  selector = _mm256_permutevar8x32_ps(selector, _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7));
  ret.re   = _mm256_blendv_ps(a.re, b.re, selector);
  ret.im   = _mm256_blendv_ps(a.im, b.im, selector);
#else
#ifdef HAVE_SSE
  ret.re = _mm_blendv_ps(a.re, b.re, selector);
  ret.im = _mm_blendv_ps(a.im, b.im, selector);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  ret.val[0] = vbslq_f32(selector, b.val[0], a.val[0]);
  ret.val[1] = vbslq_f32(selector, b.val[1], a.val[1]);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_i_t srsran_simd_i_select(simd_i_t a, simd_i_t b, simd_sel_t selector)
{
#ifdef HAVE_AVX512
  return (__m512i)_mm512_mask_blend_ps(selector, (__m512)a, (__m512)b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return (__m256i)_mm256_blendv_ps((__m256)a, (__m256)b, selector);
#else
#ifdef HAVE_SSE
  return (__m128i)_mm_blendv_ps((__m128)a, (__m128)b, selector);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vbslq_s32(selector, b, a);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

#endif /* SRSRAN_SIMD_I_SIZE*/

#if SRSRAN_SIMD_S_SIZE

#ifdef HAVE_AVX512
typedef __m512i simd_s_t;
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
typedef __m256i simd_s_t;
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
typedef __m128i simd_s_t;
#else /* HAVE_SSE */
#ifdef HAVE_NEON
typedef int16x8_t simd_s_t;
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */

static inline simd_s_t srsran_simd_s_load(const int16_t* ptr)
{
#ifdef HAVE_AVX512
  return _mm512_load_si512(ptr);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_load_si256((__m256i*)ptr);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_load_si128((__m128i*)ptr);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_s16(ptr);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_s_t srsran_simd_s_loadu(const int16_t* ptr)
{
#ifdef HAVE_AVX512
  return _mm512_loadu_si512(ptr);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_loadu_si256((__m256i*)ptr);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_loadu_si128((__m128i*)ptr);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_s16(ptr);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_s_store(int16_t* ptr, simd_s_t simdreg)
{
#ifdef HAVE_AVX512
  _mm512_store_si512(ptr, simdreg);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  _mm256_store_si256((__m256i*)ptr, simdreg);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  _mm_store_si128((__m128i*)ptr, simdreg);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_s16(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_s_storeu(int16_t* ptr, simd_s_t simdreg)
{
#ifdef HAVE_AVX512
  _mm512_storeu_si512(ptr, simdreg);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  _mm256_storeu_si256((__m256i*)ptr, simdreg);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  _mm_storeu_si128((__m128i*)ptr, simdreg);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_s16(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}
static inline simd_s_t srsran_simd_s_zero(void)
{
#ifdef HAVE_AVX512
  return _mm512_setzero_si512();
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_setzero_si256();
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_setzero_si128();
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vdupq_n_s16(0);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_s_t srsran_simd_s_mul(simd_s_t a, simd_s_t b)
{
#ifdef HAVE_AVX512
  return _mm512_mullo_epi16(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_mullo_epi16(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_mullo_epi16(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vmulq_s16(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_s_t srsran_simd_s_neg(simd_s_t a, simd_s_t b)
{
#ifdef HAVE_AVX512
  __m256i a0 = _mm512_extracti64x4_epi64(a, 0);
  __m256i a1 = _mm512_extracti64x4_epi64(a, 1);
  __m256i b0 = _mm512_extracti64x4_epi64(b, 0);
  __m256i b1 = _mm512_extracti64x4_epi64(b, 1);
  __m256i r0 = _mm256_sign_epi16(a0, b0);
  __m256i r1 = _mm256_sign_epi16(a1, b1);
  return _mm512_inserti64x4(_mm512_broadcast_i64x4(r0), r1, 1);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_sign_epi16(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_sign_epi16(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  /* Taken and modified from sse2neon.h licensed under MIT
   * Source: https://github.com/DLTcollab/sse2neon
   */
  int16x8_t zero = vdupq_n_s16(0);
  // signed shift right: faster than vclt
  // (b < 0) ? 0xffff : 0
  uint16x8_t ltMask = vreinterpretq_u16_s16(vshrq_n_s16(b, 15));
  // (b == 0) ? 0xffff : 0
  int16x8_t zeroMask = vreinterpretq_s16_u16(vceqq_s16(b, zero));
  // -a
  int16x8_t neg = vnegq_s16(a);
  // bitwise select either a or neg based on ltMask
  int16x8_t masked = vbslq_s16(ltMask, neg, a);
  // res = masked & (~zeroMask)
  int16x8_t res = vbicq_s16(masked, zeroMask);
  return res;
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_s_t srsran_simd_s_add(simd_s_t a, simd_s_t b)
{
#ifdef HAVE_AVX512
  return _mm512_add_epi16(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_add_epi16(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_add_epi16(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vaddq_s16(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_s_t srsran_simd_s_sub(simd_s_t a, simd_s_t b)
{
#ifdef HAVE_AVX512
  return _mm512_sub_epi16(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_sub_epi16(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_sub_epi16(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vsubq_s16(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

#endif /* SRSRAN_SIMD_S_SIZE */

#if SRSRAN_SIMD_C16_SIZE

typedef
#ifdef HAVE_AVX512
    struct {
  union {
    __m512i m512;
    int16_t i16[32];
  } re;
  union {
    __m512i m512;
    int16_t i16[32];
  } im;
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
    struct {
  union {
    __m256i m256;
    int16_t i16[16];
  } re;
  union {
    __m256i m256;
    int16_t i16[16];
  } im;
#else
#ifdef HAVE_SSE
    struct {
  union {
    __m128i m128;
    int16_t i16[8];
  } re;
  union {
    __m128i m128;
    int16_t i16[8];
  } im;
#else
#ifdef HAVE_NEON
    union {
  int16x8x2_t m128;
  int16_t     i16[16];
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
} simd_c16_t;

/* Fixed point precision (16-bit) functions */
static inline simd_c16_t srsran_simd_c16i_load(const c16_t* ptr)
{
  simd_c16_t ret;
#ifdef HAVE_AVX512
  __m512i in1 = _mm512_load_si512((__m512i*)(ptr));
  __m512i in2 = _mm512_load_si512((__m512i*)(ptr + 8));
  ret.re.m512 = _mm512_mask_blend_epi16(
      0xaaaaaaaa, in1, _mm512_shufflelo_epi16(_mm512_shufflehi_epi16(in2, 0b10100000), 0b10100000));
  ret.im.m512 = _mm512_mask_blend_epi16(
      0xaaaaaaaa, _mm512_shufflelo_epi16(_mm512_shufflehi_epi16(in1, 0b11110101), 0b11110101), in2);
#else /* HAVE_AVX2 */
#ifdef HAVE_AVX2
  __m256i in1 = _mm256_load_si256((__m256i*)(ptr));
  __m256i in2 = _mm256_load_si256((__m256i*)(ptr + 8));
  ret.re.m256 =
      _mm256_blend_epi16(in1, _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(in2, 0b10100000), 0b10100000), 0b10101010);
  ret.im.m256 =
      _mm256_blend_epi16(_mm256_shufflelo_epi16(_mm256_shufflehi_epi16(in1, 0b11110101), 0b11110101), in2, 0b10101010);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  __m128i in1 = _mm_load_si128((__m128i*)(ptr));
  __m128i in2 = _mm_load_si128((__m128i*)(ptr + 8));
  ret.re.m128 = _mm_blend_epi16(in1, _mm_shufflelo_epi16(_mm_shufflehi_epi16(in2, 0b10100000), 0b10100000), 0b10101010);
  ret.im.m128 = _mm_blend_epi16(_mm_shufflelo_epi16(_mm_shufflehi_epi16(in1, 0b11110101), 0b11110101), in2, 0b10101010);
#else /* HAVE_SSE*/
#ifdef HAVE_NEON
  ret.m128     = vld2q_s16((int16_t*)(ptr));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
  return ret;
}

static inline simd_c16_t srsran_simd_c16_load(const int16_t* re, const int16_t* im)
{
  simd_c16_t ret;
#ifdef HAVE_AVX2
  ret.re.m256 = _mm256_load_si256((__m256i*)(re));
  ret.im.m256 = _mm256_load_si256((__m256i*)(im));
#else
#ifdef HAVE_SSE
  ret.re.m128   = _mm_load_si128((__m128i*)(re));
  ret.im.m128   = _mm_load_si128((__m128i*)(im));
#else /* HAVE_SSE*/
#ifdef HAVE_NEON
  ret.m128.val[0] = vld1q_s16((int16_t*)(re));
  ret.m128.val[1] = vld1q_s16((int16_t*)(im));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
  return ret;
}

static inline simd_c16_t srsran_simd_c16_loadu(const int16_t* re, const int16_t* im)
{
  simd_c16_t ret;
#ifdef HAVE_AVX2
  ret.re.m256 = _mm256_loadu_si256((__m256i*)(re));
  ret.im.m256 = _mm256_loadu_si256((__m256i*)(im));
#else
#ifdef HAVE_SSE
  ret.re.m128   = _mm_loadu_si128((__m128i*)(re));
  ret.im.m128   = _mm_loadu_si128((__m128i*)(im));
#else /* HAVE_SSE*/
#ifdef HAVE_NEON
  ret.m128.val[0] = vld1q_s16((int16_t*)(re));
  ret.m128.val[1] = vld1q_s16((int16_t*)(im));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
  return ret;
}

static inline void srsran_simd_c16i_store(c16_t* ptr, simd_c16_t simdreg)
{
#ifdef HAVE_AVX2
  __m256i re_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.re.m256, 0b10110001), 0b10110001);
  __m256i im_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.im.m256, 0b10110001), 0b10110001);
  _mm256_store_si256((__m256i*)(ptr), _mm256_blend_epi16(simdreg.re.m256, im_sw, 0b10101010));
  _mm256_store_si256((__m256i*)(ptr + 8), _mm256_blend_epi16(re_sw, simdreg.im.m256, 0b10101010));
#else
#ifdef HAVE_SSE
  __m128i re_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.re.m128, 0b10110001), 0b10110001);
  __m128i im_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.im.m128, 0b10110001), 0b10110001);
  _mm_store_si128((__m128i*)(ptr), _mm_blend_epi16(simdreg.re.m128, im_sw, 0b10101010));
  _mm_store_si128((__m128i*)(ptr + 8), _mm_blend_epi16(re_sw, simdreg.im.m128, 0b10101010));
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst2q_s16((int16_t*)(ptr), simdreg.m128);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
}

static inline void srsran_simd_c16i_storeu(c16_t* ptr, simd_c16_t simdreg)
{
#ifdef HAVE_AVX2
  __m256i re_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.re.m256, 0b10110001), 0b10110001);
  __m256i im_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.im.m256, 0b10110001), 0b10110001);
  _mm256_storeu_si256((__m256i*)(ptr), _mm256_blend_epi16(simdreg.re.m256, im_sw, 0b10101010));
  _mm256_storeu_si256((__m256i*)(ptr + 8), _mm256_blend_epi16(re_sw, simdreg.im.m256, 0b10101010));
#else
#ifdef HAVE_SSE
  __m128i re_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.re.m128, 0b10110001), 0b10110001);
  __m128i im_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.im.m128, 0b10110001), 0b10110001);
  _mm_storeu_si128((__m128i*)(ptr), _mm_blend_epi16(simdreg.re.m128, im_sw, 0b10101010));
  _mm_storeu_si128((__m128i*)(ptr + 8), _mm_blend_epi16(re_sw, simdreg.im.m128, 0b10101010));
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst2q_s16((int16_t*)(ptr), simdreg.m128);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
}

static inline void srsran_simd_c16_store(int16_t* re, int16_t* im, simd_c16_t simdreg)
{
#ifdef HAVE_AVX2
  _mm256_store_si256((__m256i*)re, simdreg.re.m256);
  _mm256_store_si256((__m256i*)im, simdreg.im.m256);
#else
#ifdef HAVE_SSE
  _mm_store_si128((__m128i*)re, simdreg.re.m128);
  _mm_store_si128((__m128i*)im, simdreg.im.m128);
#else
#ifdef HAVE_NEON
  vst1q_s16((int16_t*)re, simdreg.m128.val[0]);
  vst1q_s16((int16_t*)im, simdreg.m128.val[1]);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
}

static inline void srsran_simd_c16_storeu(int16_t* re, int16_t* im, simd_c16_t simdreg)
{
#ifdef HAVE_AVX2
  _mm256_storeu_si256((__m256i*)re, simdreg.re.m256);
  _mm256_storeu_si256((__m256i*)im, simdreg.im.m256);
#else
#ifdef HAVE_SSE
  _mm_storeu_si128((__m128i*)re, simdreg.re.m128);
  _mm_storeu_si128((__m128i*)im, simdreg.im.m128);
#else
#ifdef HAVE_NEON
  vst1q_s16((int16_t*)re, simdreg.m128.val[0]);
  vst1q_s16((int16_t*)im, simdreg.m128.val[1]);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
}

static inline simd_c16_t srsran_simd_c16_prod(simd_c16_t a, simd_c16_t b)
{
  simd_c16_t ret;
#ifdef HAVE_AVX2
  ret.re.m256 = _mm256_sub_epi16(_mm256_mulhrs_epi16(a.re.m256, _mm256_slli_epi16(b.re.m256, 1)),
                                 _mm256_mulhrs_epi16(a.im.m256, _mm256_slli_epi16(b.im.m256, 1)));
  ret.im.m256 = _mm256_add_epi16(_mm256_mulhrs_epi16(a.re.m256, _mm256_slli_epi16(b.im.m256, 1)),
                                 _mm256_mulhrs_epi16(a.im.m256, _mm256_slli_epi16(b.re.m256, 1)));
#else
#ifdef HAVE_SSE
  ret.re.m128 = _mm_sub_epi16(_mm_mulhrs_epi16(a.re.m128, _mm_slli_epi16(b.re.m128, 1)),
                              _mm_mulhrs_epi16(a.im.m128, _mm_slli_epi16(b.im.m128, 1)));
  ret.im.m128 = _mm_add_epi16(_mm_mulhrs_epi16(a.re.m128, _mm_slli_epi16(b.im.m128, 1)),
                              _mm_mulhrs_epi16(a.im.m128, _mm_slli_epi16(b.re.m128, 1)));
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
  return ret;
}

static inline simd_c16_t srsran_simd_c16_add(simd_c16_t a, simd_c16_t b)
{
  simd_c16_t ret;
#ifdef HAVE_AVX2
  ret.re.m256 = _mm256_add_epi16(a.re.m256, b.re.m256);
  ret.im.m256 = _mm256_add_epi16(a.im.m256, b.im.m256);
#else
#ifdef HAVE_SSE
  ret.re.m128 = _mm_add_epi16(a.re.m128, b.re.m128);
  ret.im.m128 = _mm_add_epi16(a.im.m128, b.im.m128);
#else
#ifdef HAVE_NEON
  ret.m128.val[0] = vaddq_s16(a.m128.val[0], a.m128.val[0]);
  ret.m128.val[1] = vaddq_s16(a.m128.val[1], a.m128.val[1]);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
  return ret;
}

static inline simd_c16_t srsran_simd_c16_zero(void)
{
  simd_c16_t ret;
#ifdef HAVE_AVX2
  ret.re.m256 = _mm256_setzero_si256();
  ret.im.m256 = _mm256_setzero_si256();
#else
#ifdef HAVE_SSE
  ret.re.m128 = _mm_setzero_si128();
  ret.im.m128 = _mm_setzero_si128();
#else
#ifdef HAVE_NEON
  ret.m128.val[0] = vdupq_n_s16(0);
  ret.m128.val[1] = vdupq_n_s16(0);
#endif /* HAVE_NEON    */
#endif /* HAVE_SSE  */
#endif /* HAVE_AVX2 */
  return ret;
}

#endif /* SRSRAN_SIMD_C16_SIZE */

#if SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE

static inline simd_s_t srsran_simd_convert_2f_s(simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
  __m512 aa = _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b),
      b);
  __m512 bb = _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x04, 0x05, 0x06, 0x07, 0x0c, 0x0d, 0x0e, 0x0f, 0x14, 0x15, 0x16, 0x17, 0x1c, 0x1d, 0x1e, 0x1f),
      b);
  __m512i ai = _mm512_cvttps_epi32(aa);
  __m512i bi = _mm512_cvttps_epi32(bb);
  return _mm512_packs_epi32(ai, bi);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  __m256  aa  = _mm256_permute2f128_ps(a, b, 0x20);
  __m256  bb  = _mm256_permute2f128_ps(a, b, 0x31);
  __m256i ai  = _mm256_cvttps_epi32(aa);
  __m256i bi  = _mm256_cvttps_epi32(bb);
  return _mm256_packs_epi32(ai, bi);
#else
#ifdef HAVE_SSE
  __m128i ai      = _mm_cvttps_epi32(a);
  __m128i bi      = _mm_cvttps_epi32(b);
  return _mm_packs_epi32(ai, bi);
#else
#ifdef HAVE_NEON
  int32x4_t ai = vcvtq_s32_f32(a);
  int32x4_t bi = vcvtq_s32_f32(b);
  return (simd_s_t)vcombine_s16(vqmovn_s32(ai), vqmovn_s32(bi));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_s_t srsran_simd_convert_2f_s_round(simd_f_t a, simd_f_t b)
{
#ifdef HAVE_AVX512
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
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  __m256  aa = _mm256_round_ps(_mm256_permute2f128_ps(a, b, 0x20), _MM_FROUND_NINT);
  __m256  bb = _mm256_round_ps(_mm256_permute2f128_ps(a, b, 0x31), _MM_FROUND_NINT);
  __m256i ai = _mm256_cvtps_epi32(aa);
  __m256i bi = _mm256_cvtps_epi32(bb);
  return _mm256_packs_epi32(ai, bi);
#else
#ifdef HAVE_SSE
  __m128  aa = _mm_round_ps(a, _MM_FROUND_NINT);
  __m128  bb = _mm_round_ps(b, _MM_FROUND_NINT);
  __m128i ai = _mm_cvtps_epi32(aa);
  __m128i bi = _mm_cvtps_epi32(bb);
  return _mm_packs_epi32(ai, bi);
#else
#ifdef HAVE_NEON
  int32x4_t ai = vcvtnq_s32_f32(a);
  int32x4_t bi = vcvtnq_s32_f32(b);
  return (simd_s_t)vcombine_s16(vqmovn_s32(ai), vqmovn_s32(bi));
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

#endif /* SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_C16_SIZE */

#if SRSRAN_SIMD_B_SIZE
/* Data types */
#ifdef HAVE_AVX512
typedef __m512i simd_b_t;
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
typedef __m256i simd_b_t;
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
typedef __m128i simd_b_t;
#else /* HAVE_NEON */
#ifdef HAVE_NEON
typedef int8x16_t simd_b_t;
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */

static inline simd_b_t srsran_simd_b_load(const int8_t* ptr)
{
#ifdef HAVE_AVX512
  return _mm512_load_si512(ptr);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_load_si256((__m256i*)ptr);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_load_si128((__m128i*)ptr);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_s8(ptr);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_b_t srsran_simd_b_loadu(const int8_t* ptr)
{
#ifdef HAVE_AVX512
  return _mm512_loadu_si512(ptr);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_loadu_si256((__m256i*)ptr);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_loadu_si128((__m128i*)ptr);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_s8(ptr);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_b_store(int8_t* ptr, simd_b_t simdreg)
{
#ifdef HAVE_AVX512
  _mm512_store_si512(ptr, simdreg);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  _mm256_store_si256((__m256i*)ptr, simdreg);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  _mm_store_si128((__m128i*)ptr, simdreg);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_s8(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline void srsran_simd_b_storeu(int8_t* ptr, simd_b_t simdreg)
{
#ifdef HAVE_AVX512
  _mm512_storeu_si512(ptr, simdreg);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  _mm256_storeu_si256((__m256i*)ptr, simdreg);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  _mm_storeu_si128((__m128i*)ptr, simdreg);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_s8(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_b_t srsran_simd_b_xor(simd_b_t a, simd_b_t b)
{
#ifdef HAVE_AVX512
  return _mm512_xor_epi32(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_xor_si256(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_xor_si128(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return veorq_s8(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_b_t srsran_simd_b_add(simd_b_t a, simd_b_t b)
{
#ifdef HAVE_AVX512
  return _mm512_adds_epi8(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_adds_epi8(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_adds_epi8(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vqaddq_s8(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_b_t srsran_simd_b_sub(simd_b_t a, simd_b_t b)
{
#ifdef HAVE_AVX512
  return _mm512_subs_epi8(a, b);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_subs_epi8(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_subs_epi8(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  return vqsubq_s8(a, b);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

static inline simd_b_t srsran_simd_b_neg(simd_b_t a, simd_b_t b)
{
#ifdef HAVE_AVX512
  __m256i a0 = _mm512_extracti64x4_epi64(a, 0);
  __m256i a1 = _mm512_extracti64x4_epi64(a, 1);
  __m256i b0 = _mm512_extracti64x4_epi64(b, 0);
  __m256i b1 = _mm512_extracti64x4_epi64(b, 1);
  __m256i r0 = _mm256_sign_epi8(a0, b0);
  __m256i r1 = _mm256_sign_epi8(a1, b1);
  return _mm512_inserti64x4(_mm512_broadcast_i64x4(r0), r1, 1);
#else /* HAVE_AVX512 */
#ifdef HAVE_AVX2
  return _mm256_sign_epi8(a, b);
#else /* HAVE_AVX2 */
#ifdef HAVE_SSE
  return _mm_sign_epi8(a, b);
#else /* HAVE_SSE */
#ifdef HAVE_NEON
  /* Taken and modified from sse2neon.h licensed under MIT
   * Source: https://github.com/DLTcollab/sse2neon
   */
  int8x16_t zero = vdupq_n_s8(0);
  // signed shift right: faster than vclt
  // (b < 0) ? 0xff : 0
  uint8x16_t ltMask = vreinterpretq_u8_s8(vshrq_n_s8(b, 7));
  // (b == 0) ? 0xff : 0
  int8x16_t zeroMask = vreinterpretq_s8_u8(vceqq_s8(b, zero));
  // -a
  int8x16_t neg = vnegq_s8(a);
  // bitwise select either a or neg based on ltMask
  int8x16_t masked = vbslq_s8(ltMask, neg, a);
  // res = masked & (~zeroMask)
  int8x16_t res = vbicq_s8(masked, zeroMask);
  return res;
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* HAVE_AVX2 */
#endif /* HAVE_AVX512 */
}

#endif /*SRSRAN_SIMD_B_SIZE */

} // namespace srsran
