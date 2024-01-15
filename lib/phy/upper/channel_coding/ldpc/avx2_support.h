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
/// \brief AVX2 support for LDPC.
///
/// Builds upon \ref simd_support.h and specializes functions and templates for AVX2 registers.

#pragma once

#include "simd_support.h"

namespace srsran {

namespace detail {

template <typename simdWrapper, typename storageType>
simd256_type simd_span<simdWrapper, storageType>::get_at(help_type<simd256_wrapper> /**/, unsigned pos) const
{
  srsran_assert(pos < view_length, "Index {} out of bound.", pos);
  return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(array_ptr) + pos);
}

template <typename simdWrapper, typename storageType>
void simd_span<simdWrapper, storageType>::set_at(unsigned pos, simd256_type val)
{
  static_assert(SIMD_SIZE_BYTE == AVX2_SIZE_BYTE, "Cannot set an AVX512 vector with an AVX2 vector.");
  srsran_assert(pos < view_length, "Index {} out of bound.", pos);
  _mm256_storeu_si256(reinterpret_cast<__m256i*>(array_ptr) + pos, val);
}

} // namespace detail

namespace mm256 {

using avx2_span       = detail::simd_span<detail::simd256_wrapper, int8_t>;
using avx2_const_span = detail::simd_span<detail::simd256_wrapper, const int8_t>;

/// \brief Scales packed 8-bit integers in \c a by the scaling factor \c sf.
///
/// Values of \c a larger than \c max or smaller than \c -max are forwarded unaltered.
/// \param[in] a   Vector of packed 8-bit integers.
/// \param[in] sf  Scaling factor (0, 1].
/// \param[in] max Maximum input value (in absolute value) to which the scaling is applied [0, 127).
/// \return    Vector of packed 8-bit integers with the scaling result.
inline __m256i scale_epi8(__m256i a, float sf, uint8_t max)
{
  srsran_assert((sf > 0) && (sf <= 1), "Scaling factor out of range.");
  srsran_assert(max < 127, "Parameter max out of range.");

  if (sf >= .9999) {
    return a;
  }

  static const __m256i mask_even_epi8 = _mm256_set1_epi16(0x00ff);
  // FLOAT2INT = 2^16 = 65536
  static constexpr unsigned FLOAT2INT = 1U << 16U;
  const __m256i             MAX_epi8  = _mm256_set1_epi8(max);
  const __m256i             MIN_epi8  = _mm256_set1_epi8(-max);

  // Create a register filled with copies of the scaling factor times FLOAT2INT. Note that the upper bound on sf implies
  // that (sf * FLOAT2INT) can be represented over 16 bits.
  __m256i sf_epi16 = _mm256_set1_epi16(static_cast<uint16_t>(sf * FLOAT2INT));
  // Even bytes in a, alternated with a zero-valued byte.
  __m256i even_epi16 = _mm256_and_si256(a, mask_even_epi8);
  // Odd bytes in a, moved to the even positions and alternated with a zero-valued byte.
  __m256i odd_epi16 = _mm256_srli_epi16(a, 8);

  // This is equivalent to multiply each byte by sf: indeed, we multiply each byte by (sf * FLOAT2INT), store the result
  // in over 32 bit and pick only the 16 most significant bits (i.e., we divide by FLOAT2INT).
  __m256i p_even_epi16 = _mm256_mulhi_epu16(even_epi16, sf_epi16);
  __m256i p_odd_epi16  = _mm256_mulhi_epu16(odd_epi16, sf_epi16);

  // Move the odd bits back to their original positions.
  p_odd_epi16 = _mm256_slli_epi16(p_odd_epi16, 8);

  // Combine even and odd bits. Note that the "odd" bytes of p_even_epi16 are equal to 0 (the result of multiplying a
  // byte by a 16-bit value will occupy at most 24 of the 32 bits).
  __m256i product_epi8 = _mm256_xor_si256(p_even_epi16, p_odd_epi16);

  // Replace values corresponding to "large" inputs with their original values.
  __m256i mask_epi8 = _mm256_cmpgt_epi8(a, MAX_epi8);
  product_epi8      = _mm256_blendv_epi8(product_epi8, a, mask_epi8);
  mask_epi8         = _mm256_cmpgt_epi8(MIN_epi8, a);
  product_epi8      = _mm256_blendv_epi8(product_epi8, a, mask_epi8);
  return product_epi8;
}

} // namespace mm256
} // namespace srsran