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

#include "srsran/srsvec/conversion.h"
#include "srsran/srsvec/simd.h"

using namespace srsran;
using namespace srsvec;

static void convert_fi_simd(const float* x, int16_t* z, float scale, unsigned len)
{
  unsigned i = 0;

#if SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE
  simd_f_t s = srsran_simd_f_set1(scale);
  if (SIMD_IS_ALIGNED(x) && SIMD_IS_ALIGNED(z)) {
    for (; i + SRSRAN_SIMD_S_SIZE < len + 1; i += SRSRAN_SIMD_S_SIZE) {
      simd_f_t a = srsran_simd_f_load(x + i);
      simd_f_t b = srsran_simd_f_load(x + i + SRSRAN_SIMD_F_SIZE);

      simd_f_t sa = srsran_simd_f_mul(a, s);
      simd_f_t sb = srsran_simd_f_mul(b, s);

      simd_s_t i16 = srsran_simd_convert_2f_s(sa, sb);

      srsran_simd_s_store(z + i, i16);
    }
  } else {
    for (; i + SRSRAN_SIMD_S_SIZE < len + 1; i += SRSRAN_SIMD_S_SIZE) {
      simd_f_t a = srsran_simd_f_loadu(x + i);
      simd_f_t b = srsran_simd_f_loadu(x + i + SRSRAN_SIMD_F_SIZE);

      simd_f_t sa = srsran_simd_f_mul(a, s);
      simd_f_t sb = srsran_simd_f_mul(b, s);

      simd_s_t i16 = srsran_simd_convert_2f_s(sa, sb);

      srsran_simd_s_storeu(z + i, i16);
    }
  }
#endif /* SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE */

  for (; i != len; ++i) {
    z[i] = static_cast<int16_t>(std::round(x[i] * scale));
  }
}

static void convert_if_simd(float* z, const int16_t* x, float scale, unsigned len)
{
  unsigned    i    = 0;
  const float gain = 1.0f / scale;

#if defined(__AVX__) && defined(__AVX512F__)
  // Load the scale factor into a vector register.
  __m512 scale512 = _mm512_set1_ps(gain);

  // Process 16 elements at a time (512 bits / 32 bits per float = 16 floats).
  for (unsigned i_end = (len / 16) * 16; i != i_end; i += 16) {
    // Load 16 int16_t elements into a 256-bit vector register.
    __m256i input_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(x + i));

    // Convert the int16_t elements to float and scale them.
    __m512 float_vec = _mm512_cvtepi32_ps(_mm512_cvtepi16_epi32(input_vec));
    float_vec        = _mm512_mul_ps(float_vec, scale512);

    // Store the result back to memory.
    _mm512_storeu_ps(z + i, float_vec);
  }
#if defined(__AVX512VL__)
  {
    unsigned remainder = len % 16;

    // Select the LSB values.
    __mmask16 mask = (1 << remainder) - 1;

    // Load 16 int16_t elements into a 256-bit vector register.
    __m256i input_vec = _mm256_maskz_loadu_epi16(mask, reinterpret_cast<const __m256i*>(x + i));

    // Convert the int16_t elements to float and scale them.
    __m512 float_vec = _mm512_maskz_cvtepi32_ps(mask, _mm512_maskz_cvtepi16_epi32(mask, input_vec));
    float_vec        = _mm512_mul_ps(float_vec, scale512);

    // Store the result back to memory.
    _mm512_mask_storeu_ps(z + i, mask, float_vec);
    return;
  }
#endif // defined(__AVX512VL__)
#endif // defined(__AVX__) && defined(__AVX512F__)

#if defined(__AVX__) && defined(__AVX2__)
  // Load the scale factor into a vector register.
  __m256 scale256 = _mm256_set1_ps(gain);

  // Process 8 elements at a time (256 bits / 32 bits per float = 8 floats).
  for (unsigned i_end = (len / 8) * 8; i != i_end; i += 8) {
    // Load 8 int16_t elements into a 128-bit vector register.
    __m128i input_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(x + i));

    // Convert the int16_t elements to float and scale them
    __m256 float_vec = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(input_vec));
    float_vec        = _mm256_mul_ps(float_vec, scale256);

    // Store the result back to memory.
    _mm256_storeu_ps(z + i, float_vec);
  }
#endif // defined(__AVX__) && defined(__AVX2__)

  for (; i != len; ++i) {
    z[i] = static_cast<float>(x[i]) * gain;
  }
}

static void convert_cbf16_to_cf_simd(cf_t* out, const cbf16_t* in, unsigned len)
{
  unsigned i = 0;

#if SRSRAN_SIMD_CF_SIZE
  for (unsigned end = (len / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i != end; i += SRSRAN_SIMD_CF_SIZE) {
    srsran_simd_cfi_storeu(out + i, srsran_simd_cbf16_loadu(in + i));
  }
#endif // SRSRAN_SIMD_CF_SIZE

  for (; i != len; ++i) {
    out[i] = to_cf(in[i]);
  }
}

static void convert_bf16_to_f_simd(float* out, const bf16_t* in, unsigned len)
{
  unsigned i = 0;

#if defined(__ARM_NEON) && defined(__ARM_FEATURE_BF16_VECTOR_ARITHMETIC)
  for (unsigned i_end = (len / 4) * 4; i != i_end; i += 4) {
    // Load 4 bf16 elements into a 64-bit vector register.
    bfloat16x4_t input_vec = vld1_bf16(reinterpret_cast<const bfloat16_t*>(in + i));
    // Convert bf16 to single-precision floating point.
    float32x4_t output_vec = vcvt_f32_bf16(input_vec);
    // Store the result back to memory.
    srsran_simd_f_storeu(out + i, output_vec);
  }
#else // __ARM_FEATURE_BF16

#if SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE
  for (unsigned end = (len / SRSRAN_SIMD_S_SIZE) * SRSRAN_SIMD_S_SIZE; i != end; i += SRSRAN_SIMD_S_SIZE) {
    simd_f_t even, odd;
    // Load and unpack bf16 values into two vectors of floats: even part of each 32-bit register storing two bf16 values
    // goes into the first simd register, odd part - into the second one.
    srsran_simd_bf16_loadu(even, odd, in + i);
    srsran_simd_f_storeu_interleaved(out + i, even, odd);
  }
#endif // SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE
#endif // __ARM_FEATURE_BF16

  for (; i != len; ++i) {
    out[i] = to_float(in[i]);
  }
}

static void convert_f_to_bf16_simd(bf16_t* out, const float* in, unsigned len)
{
  unsigned i = 0;

#if SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE
  constexpr unsigned FLOATS_PER_ITERATION = 2 * SRSRAN_SIMD_F_SIZE;

  for (unsigned end = (len / FLOATS_PER_ITERATION) * FLOATS_PER_ITERATION; i != end; i += FLOATS_PER_ITERATION) {
    simd_f_t float_vec_1 = srsran_simd_f_loadu(in + i);
    simd_f_t float_vec_2 = srsran_simd_f_loadu(in + i + SRSRAN_SIMD_F_SIZE);

    // Convert float to brain float and store the result back to memory.
    srsran_simd_bf16_storeu(out + i, float_vec_1, float_vec_2);
  }
#endif // SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE

  for (; i != len; ++i) {
    out[i] = to_bf16(in[i]);
  }
}

static void convert_bf16_to_int16_simd(int16_t* out, const bf16_t* in, float scale, unsigned len)
{
  unsigned i = 0;

#if SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE
  for (unsigned end = (len / SRSRAN_SIMD_S_SIZE) * SRSRAN_SIMD_S_SIZE; i != end; i += SRSRAN_SIMD_S_SIZE) {
    simd_f_t temp_even;
    simd_f_t temp_odd;
    // Load and unpack bf16 values into two vectors of floats: even part of each 32-bit register storing two bf16 values
    // goes into the first simd register, odd part - into the second one.
    srsran_simd_bf16_loadu(temp_even, temp_odd, in + i);

    // Multiply with the scaling factor.
    simd_f_t s           = srsran_simd_f_set1(scale);
    simd_f_t scaled_even = srsran_simd_f_mul(temp_even, s);
    simd_f_t scaled_odd  = srsran_simd_f_mul(temp_odd, s);

    // Convert float to int16.
    simd_s_t i16 = srsran_simd_convert_2f_interleaved_s(scaled_even, scaled_odd);

    // Store the resulting int16 vector.
    srsran_simd_s_storeu(out + i, i16);
  }
#endif // SRSRAN_SIMD_F_SIZE && SRSRAN_SIMD_S_SIZE

  for (; i != len; ++i) {
    out[i] = static_cast<int16_t>(std::round(to_float(in[i]) * scale));
  }
}

static void convert_int16_to_bf16_simd(bf16_t* out, const int16_t* in, float scale, unsigned len)
{
  unsigned    i    = 0;
  const float gain = 1.0f / scale;

#if defined(__AVX__) && defined(__AVX512F__)
  // Load the scale factor into a vector register.
  __m512 scale512 = _mm512_set1_ps(gain);

  // Process 32 elements at a time (512 bits / 16 bits per brain float = 32 floats).
  for (unsigned i_end = (len / 32) * 32; i != i_end; i += 32) {
    // Load 32 int16_t elements into a 256-bit vector register.
    __m256i input_vec_1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i));
    __m256i input_vec_2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i + 16));

    // Convert the int16_t elements to float and scale them.
    __m512 float_vec_1 = _mm512_cvtepi32_ps(_mm512_cvtepi16_epi32(input_vec_1));
    __m512 float_vec_2 = _mm512_cvtepi32_ps(_mm512_cvtepi16_epi32(input_vec_2));
    float_vec_1        = _mm512_mul_ps(float_vec_1, scale512);
    float_vec_2        = _mm512_mul_ps(float_vec_2, scale512);

    // Convert float to brain float and store the result back to memory.
    srsran_simd_bf16_storeu(out + i, float_vec_1, float_vec_2);
  }

  // Process 16 elements at a time.
  for (unsigned i_end = (len / 16) * 16; i < i_end; i += 16) {
    // Load 16 int16_t elements into a 256-bit vector register.
    __m256i input_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i));

    // Convert the int16_t elements to float and scale them.
    __m512 float_vec = _mm512_cvtepi32_ps(_mm512_cvtepi16_epi32(input_vec));
    float_vec        = _mm512_mul_ps(float_vec, scale512);

    // Convert float to brain float, the second half of the resulting vector is empty.
    __m512i bf16_vec = srsran_simd_convert_1f_bf16(float_vec);

    // Store first half of the resulting bf16 vector to memory.
    _mm512_mask_storeu_epi32(out + i, 0x00ff, bf16_vec);
  }
#if defined(__AVX512VL__) && defined(__AVX512BW__)
  {
    unsigned remainder = len % 16;

    // Select the LSB values.
    __mmask16 mask = (1 << remainder) - 1;

    // Load remaining int16_t elements into a 256-bit vector register.
    __m256i input_vec = _mm256_maskz_loadu_epi16(mask, reinterpret_cast<const __m256i*>(in + i));

    // Convert the int16_t elements to float and scale them.
    __m512 float_vec = _mm512_maskz_cvtepi32_ps(mask, _mm512_maskz_cvtepi16_epi32(mask, input_vec));
    float_vec        = _mm512_mul_ps(float_vec, scale512);

    // Convert float to brain float, the second half of the resulting vector is empty.
    __m512i bf16_vec = srsran_simd_convert_1f_bf16(float_vec);

    // Store the result back to memory.
    _mm512_mask_storeu_epi16(out + i, static_cast<__mmask32>(mask), bf16_vec);
    return;
  }
#endif // defined(__AVX512VL__)
#else  // defined(__AVX__) && defined(__AVX512F__)

#if defined(__AVX__) && defined(__AVX2__)
  // Load the scale factor into a vector register.
  __m256 scale256 = _mm256_set1_ps(gain);

  // Process 16 elements at a time (256 bits /16 bits per float = 16 floats).
  for (unsigned i_end = (len / 16) * 16; i != i_end; i += 16) {
    // Load 8 int16_t elements into two 128-bit vector registers.
    __m128i input_vec_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in + i));
    __m128i input_vec_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in + i + 8));

    // Convert the int16_t elements to float and scale them
    __m256 float_vec_1 = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(input_vec_1));
    __m256 float_vec_2 = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(input_vec_2));
    float_vec_1        = _mm256_mul_ps(float_vec_1, scale256);
    float_vec_2        = _mm256_mul_ps(float_vec_2, scale256);

    // Convert float to brain float and store the result back to memory.
    srsran_simd_bf16_storeu(out + i, float_vec_1, float_vec_2);
  }
#endif // defined(__AVX__) && defined(__AVX2__)
#endif // defined(__AVX__) && defined(__AVX512F__)

#if defined(__ARM_NEON)
  // Load the scale factor into a vector register.
  float32x4_t scale_f32 = vdupq_n_f32(gain);

  // Process 8 elements at a time (128 bits / 16 bits per brain float = 8 floats).
  for (unsigned i_end = (len / 8) * 8; i != i_end; i += 8) {
    // Load 8 int16_t elements into a 128-bit vector register.
    int16x8_t input_vec = vld1q_s16(in + i);

    // Convert the int16_t elements to float and scale them.
    float32x4_t float_vec_1 = vcvtq_f32_s32(vmovl_s16(vget_low_s16(input_vec)));
    float32x4_t float_vec_2 = vcvtq_f32_s32(vmovl_high_s16(input_vec));
    float_vec_1             = vmulq_f32(float_vec_1, scale_f32);
    float_vec_2             = vmulq_f32(float_vec_2, scale_f32);

    // Convert float to brain float and store the result back to memory.
    srsran_simd_bf16_storeu(out + i, float_vec_1, float_vec_2);
  }
#endif // defined(__ARM_NEON)
  for (; i != len; ++i) {
    out[i] = to_bf16(static_cast<float>(in[i]) * gain);
  }
}

static void convert_scaled_int16_to_bf16_simd(bf16_t* out, const int16_t* in, const float* in_gain, unsigned len)
{
  unsigned i = 0;

#if defined(__AVX__) && defined(__AVX512F__)
  // Process 32 elements at a time (512 bits / 16 bits per brain float = 32 floats).
  for (unsigned i_end = (len / 32) * 32; i != i_end; i += 32) {
    // Load 32 int16_t elements into a 256-bit vector register.
    __m256i input_vec_1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i));
    __m256i input_vec_2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i + 16));

    // Load the scale factor into a vector register.
    __m512 scale_vec_1 = _mm512_loadu_ps(in_gain + i);
    __m512 scale_vec_2 = _mm512_loadu_ps(in_gain + i + 16);

    // Convert the int16_t elements to float and scale them.
    __m512 float_vec_1 = _mm512_cvtepi32_ps(_mm512_cvtepi16_epi32(input_vec_1));
    __m512 float_vec_2 = _mm512_cvtepi32_ps(_mm512_cvtepi16_epi32(input_vec_2));
    float_vec_1        = _mm512_mul_ps(float_vec_1, scale_vec_1);
    float_vec_2        = _mm512_mul_ps(float_vec_2, scale_vec_2);

    // Convert float to brain float and store the result back to memory.
    srsran_simd_bf16_storeu(out + i, float_vec_1, float_vec_2);
  }

  // Process 16 elements at a time.
  for (unsigned i_end = (len / 16) * 16; i < i_end; i += 16) {
    // Load 16 int16_t elements into a 256-bit vector register.
    __m256i input_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i));

    // Load the scale factor into a vector register.
    __m512 scale_vec = _mm512_loadu_ps(in_gain + i);

    // Convert the int16_t elements to float and scale them.
    __m512 float_vec = _mm512_cvtepi32_ps(_mm512_cvtepi16_epi32(input_vec));
    float_vec        = _mm512_mul_ps(float_vec, scale_vec);

    // Convert float to brain float, the second half of the resulting vector is empty.
    __m512i bf16_vec = srsran_simd_convert_1f_bf16(float_vec);

    // Store first half of the resulting bf16 vector to memory.
    _mm512_mask_storeu_epi32(out + i, 0x00ff, bf16_vec);
  }
#if defined(__AVX512VL__) && defined(__AVX512BW__)
  {
    unsigned remainder = len % 16;

    // Select the LSB values.
    __mmask16 mask = (1 << remainder) - 1;

    // Load remaining int16_t elements into a 256-bit vector register.
    __m256i input_vec = _mm256_maskz_loadu_epi16(mask, reinterpret_cast<const __m256i*>(in + i));

    // Load remaining scale factors into a 512-bit vector register.
    __m512 scale_vec = _mm512_maskz_loadu_ps(mask, in_gain + i);

    // Convert the int16_t elements to float and scale them.
    __m512 float_vec = _mm512_maskz_cvtepi32_ps(mask, _mm512_maskz_cvtepi16_epi32(mask, input_vec));
    float_vec        = _mm512_mul_ps(float_vec, scale_vec);

    // Convert float to brain float, the second half of the resulting vector is empty.
    __m512i bf16_vec = srsran_simd_convert_1f_bf16(float_vec);

    // Store the result back to memory.
    _mm512_mask_storeu_epi16(out + i, static_cast<__mmask32>(mask), bf16_vec);
    return;
  }
#endif // defined(__AVX512VL__)
#else  // defined(__AVX__) && defined(__AVX512F__)

#if defined(__AVX__) && defined(__AVX2__)
  // Process 16 elements at a time (256 bits /16 bits per float = 16 floats).
  for (unsigned i_end = (len / 16) * 16; i != i_end; i += 16) {
    // Load 8 int16_t elements into two 128-bit vector registers.
    __m128i input_vec_1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in + i));
    __m128i input_vec_2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in + i + 8));

    // Load the scale factor into a vector register.
    __m256 scale_vec_1 = _mm256_loadu_ps(in_gain + i);
    __m256 scale_vec_2 = _mm256_loadu_ps(in_gain + i + 8);

    // Convert the int16_t elements to float and scale them
    __m256 float_vec_1 = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(input_vec_1));
    __m256 float_vec_2 = _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(input_vec_2));
    float_vec_1        = _mm256_mul_ps(float_vec_1, scale_vec_1);
    float_vec_2        = _mm256_mul_ps(float_vec_2, scale_vec_2);

    // Convert float to brain float and store the result back to memory.
    srsran_simd_bf16_storeu(out + i, float_vec_1, float_vec_2);
  }
#endif // defined(__AVX__) && defined(__AVX2__)
#endif // defined(__AVX__) && defined(__AVX512F__)

#if defined(__ARM_NEON)
  // Process 8 elements at a time (128 bits / 16 bits per brain float = 8 floats).
  for (unsigned i_end = (len / 8) * 8; i != i_end; i += 8) {
    // Load 8 int16_t elements into a 128-bit vector register.
    int16x8_t input_vec = vld1q_s16(in + i);

    // Load the scale factor into a vector register.
    float32x4_t scale_f32_1 = vld1q_f32(in_gain + i);
    float32x4_t scale_f32_2 = vld1q_f32(in_gain + i + 4);

    // Convert the int16_t elements to float and scale them.
    float32x4_t float_vec_1 = vcvtq_f32_s32(vmovl_s16(vget_low_s16(input_vec)));
    float32x4_t float_vec_2 = vcvtq_f32_s32(vmovl_high_s16(input_vec));
    float_vec_1             = vmulq_f32(float_vec_1, scale_f32_1);
    float_vec_2             = vmulq_f32(float_vec_2, scale_f32_2);

    // Convert float to brain float and store the result back to memory.
    srsran_simd_bf16_storeu(out + i, float_vec_1, float_vec_2);
  }
#endif // defined(__ARM_NEON)
  for (; i != len; ++i) {
    out[i] = to_bf16(static_cast<float>(in[i]) * in_gain[i]);
  }
}

void srsran::srsvec::convert(span<int16_t> z, span<const cf_t> x, float scale)
{
  srsran_assert(2 * x.size() == z.size(), "Invalid input or output span sizes");

  convert_fi_simd(reinterpret_cast<const float*>(x.data()), z.data(), scale, z.size());
}

void srsran::srsvec::convert(span<ci16_t> z, span<const cbf16_t> x, float scale)
{
  srsran_assert(x.size() == z.size(), "Invalid input or output span sizes");

  convert_bf16_to_int16_simd(
      reinterpret_cast<int16_t*>(z.data()), reinterpret_cast<const bf16_t*>(x.data()), scale, z.size());
}

void srsran::srsvec::convert(span<cf_t> z, span<const int16_t> x, float scale)
{
  srsran_assert(x.size() == 2 * z.size(), "Invalid input or output span sizes");

  convert_if_simd(reinterpret_cast<float*>(z.data()), x.data(), scale, x.size());
}

void srsran::srsvec::convert(span<float> z, span<const int16_t> x, float scale)
{
  srsran_assert(x.size() == z.size(), "Invalid input or output span sizes");

  convert_if_simd(z.data(), x.data(), scale, x.size());
}

void srsran::srsvec::convert(span<int16_t> z, span<const float> x, float scale)
{
  srsran_assert(x.size() == z.size(), "Invalid input or output span sizes");

  convert_fi_simd(x.data(), z.data(), scale, z.size());
}

void srsran::srsvec::convert(span<cf_t> out, span<const cbf16_t> in)
{
  srsran_assert(in.size() == out.size(), "Invalid input or output span sizes");
  convert_cbf16_to_cf_simd(out.data(), in.data(), in.size());
}

void srsran::srsvec::convert(span<float> out, span<const bf16_t> in)
{
  srsran_assert(in.size() == out.size(), "Invalid input or output span sizes");
  convert_bf16_to_f_simd(out.data(), in.data(), in.size());
}

void srsran::srsvec::convert(span<cbf16_t> out, span<const cf_t> in)
{
  srsran_assert(in.size() == out.size(), "Invalid input or output span sizes");
  convert_f_to_bf16_simd(
      reinterpret_cast<bf16_t*>(out.data()), reinterpret_cast<const float*>(in.data()), 2 * in.size());
}

void srsran::srsvec::convert(span<bf16_t> out, span<const float> in)
{
  srsran_assert(in.size() == out.size(), "Invalid input or output span sizes");
  convert_f_to_bf16_simd(out.data(), in.data(), in.size());
}

void srsran::srsvec::convert(span<int16_t> z, span<const srsran::cbf16_t> x, float scale)
{
  srsran_assert(2 * x.size() == z.size(), "Invalid input or output span sizes");

  convert_bf16_to_int16_simd(z.data(), reinterpret_cast<const bf16_t*>(x.data()), scale, z.size());
}

void srsran::srsvec::convert(span<cbf16_t> z, span<const int16_t> x, float scale)
{
  srsran_assert(x.size() == 2 * z.size(), "Invalid input or output span sizes");

  convert_int16_to_bf16_simd(reinterpret_cast<bf16_t*>(z.data()), x.data(), scale, x.size());
}

void srsran::srsvec::convert(span<cbf16_t> z, span<const int16_t> x, span<const float> scale)
{
  srsran_assert(x.size() == 2 * z.size(), "Invalid input or output span sizes");
  srsran_assert(x.size() == scale.size(), "Invalid input data or input scaling span sizes");

  convert_scaled_int16_to_bf16_simd(reinterpret_cast<bf16_t*>(z.data()), x.data(), scale.data(), x.size());
}

void srsran::srsvec::convert(span<int16_t> z, span<const bf16_t> x, float scale)
{
  srsran_assert(x.size() == z.size(), "Invalid input or output span sizes");

  convert_bf16_to_int16_simd(z.data(), x.data(), scale, z.size());
}

void srsran::srsvec::convert(span<bf16_t> z, span<const int16_t> x, float scale)
{
  srsran_assert(x.size() == z.size(), "Invalid input or output span sizes");

  convert_int16_to_bf16_simd(z.data(), x.data(), scale, z.size());
}

void srsran::srsvec::convert(span<ci16_t> z, span<const cf_t> x, float scale)
{
  srsran_assert(x.size() == z.size(), "Invalid input or output span sizes");

  convert_fi_simd(reinterpret_cast<const float*>(x.data()), reinterpret_cast<int16_t*>(z.data()), scale, z.size() * 2);
}

void srsran::srsvec::convert(span<cf_t> z, span<const ci16_t> x, float scale)
{
  srsran_assert(x.size() == z.size(), "Invalid input or output span sizes");

  convert_if_simd(reinterpret_cast<float*>(z.data()), reinterpret_cast<const int16_t*>(x.data()), scale, z.size() * 2);
}
