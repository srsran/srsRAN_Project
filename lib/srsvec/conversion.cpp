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

#include "srsran/srsvec/conversion.h"
#include "simd.h"

using namespace srsran;
using namespace srsvec;

static inline void convert_fi_simd(const float* x, int16_t* z, float scale, unsigned len)
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

static inline void convert_if_simd(float* z, const int16_t* x, float scale, unsigned len)
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

    // Store the result back to memory
    _mm256_storeu_ps(z + i, float_vec);
  }
#endif // defined(__AVX__) && defined(__AVX2__)

  for (; i != len; ++i) {
    z[i] = static_cast<float>(x[i]) * gain;
  }
}

void srsran::srsvec::convert(span<const cf_t> x, float scale, span<int16_t> z)
{
  srsran_assert(2 * x.size() == z.size(), "Invalid input or output span sizes");

  convert_fi_simd(reinterpret_cast<const float*>(x.data()), z.data(), scale, z.size());
}

void srsran::srsvec::convert(span<const int16_t> x, float scale, span<cf_t> z)
{
  srsran_assert(x.size() == 2 * z.size(), "Invalid input or output span sizes");

  convert_if_simd(reinterpret_cast<float*>(z.data()), x.data(), scale, x.size());
}

void srsran::srsvec::convert(span<const int16_t> x, float scale, span<float> z)
{
  srsran_assert(x.size() == z.size(), "Invalid input or output span sizes");

  convert_if_simd(z.data(), x.data(), scale, x.size());
}

void srsran::srsvec::convert(span<const float> x, float scale, span<int16_t> z)
{
  srsran_assert(x.size() == z.size(), "Invalid input or output span sizes");

  convert_fi_simd(x.data(), z.data(), scale, z.size());
}
