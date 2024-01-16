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
/// \brief AVX512 support for LDPC.
///
/// Builds upon \ref simd_support.h and specializes functions and templates for AVX512 registers.

#pragma once

#include "avx2_support.h"

namespace srsran {

namespace detail {

template <typename simdWrapper, typename storageType>
simd512_type simd_span<simdWrapper, storageType>::get_at(help_type<simd512_wrapper> /**/, unsigned pos) const
{
  srsran_assert(pos < view_length, "Index {} out of bound.", pos);
  return _mm512_loadu_si512(reinterpret_cast<const __m512i*>(array_ptr) + pos);
}

template <typename simdWrapper, typename storageType>
void simd_span<simdWrapper, storageType>::set_at(unsigned pos, simd512_type val)
{
  static_assert(SIMD_SIZE_BYTE == AVX512_SIZE_BYTE, "Cannot set an AVX2 vector with an AVX512 vector.");
  srsran_assert(pos < view_length, "Index {} out of bound.", pos);
  _mm512_storeu_si512(reinterpret_cast<__m512i*>(array_ptr) + pos, val);
}

} // namespace detail

namespace mm512 {

using avx512_span       = detail::simd_span<detail::simd512_wrapper, int8_t>;
using avx512_const_span = detail::simd_span<detail::simd512_wrapper, const int8_t>;

/// \brief Scales packed 8-bit integers in \c a by the scaling factor \c sf.
///
/// Values of \c a larger than \c max or smaller than \c -max are forwarded unaltered.
/// \param[in] a   Vector of packed 8-bit integers.
/// \param[in] sf  Scaling factor (0, 1].
/// \param[in] max Maximum input value (in absolute value) to which the scaling is applied [0, 127).
/// \return    Vector of packed 8-bit integers with the scaling result.
inline __m512i scale_epi8(__m512i a, float sf, uint8_t max)
{
  srsran_assert((sf > 0) && (sf <= 1), "Scaling factor out of range.");
  srsran_assert(max < 127, "Parameter max out of range.");

  if (sf >= .9999) {
    return a;
  }

  static const __m512i mask_even_epi8 = _mm512_set1_epi16(0x00ff);
  const __m512i        MAX_epi8       = _mm512_set1_epi8(max);
  const __m512i        MIN_epi8       = _mm512_set1_epi8(-max);
  // FLOAT2INT = 2^16 = 65536
  static constexpr unsigned FLOAT2INT = 1U << 16U;

  // Create a register filled with copies of the scaling factor times FLOAT2INT. Note that the upper bound on sf implies
  // that (sf * FLOAT2INT) can be represented over 16 bits.
  __m512i sf_epi16 = _mm512_set1_epi16(static_cast<uint16_t>(sf * FLOAT2INT));
  // Even bytes in a, alternated with a zero-valued byte.
  __m512i even_epi16 = _mm512_and_si512(a, mask_even_epi8);
  // Odd bytes in a, moved to the even positions and alternated with a zero-valued byte.
  __m512i odd_epi16 = _mm512_srli_epi16(a, 8);

  // This is equivalent to multiply each byte by sf: indeed, we multiply each byte by (sf * FLOAT2INT), store the result
  // in over 32 bit and pick only the 16 most significant bits (i.e., we divide by FLOAT2INT).
  __m512i p_even_epi16 = _mm512_mulhi_epu16(even_epi16, sf_epi16);
  __m512i p_odd_epi16  = _mm512_mulhi_epu16(odd_epi16, sf_epi16);

  // Move the odd bits back to their original positions.
  p_odd_epi16 = _mm512_slli_epi16(p_odd_epi16, 8);

  // Combine even and odd bits. Note that the "odd" bytes of p_even_epi16 are equal to 0 (the result of multiplying a
  // byte by a 16-bit value will occupy at most 24 of the 32 bits).
  __m512i product_epi8 = _mm512_xor_si512(p_even_epi16, p_odd_epi16);

  // Replace values corresponding to "large" inputs with their original values.
  __mmask64 mask_epi8 = _mm512_cmpgt_epi8_mask(a, MAX_epi8);
  product_epi8        = _mm512_mask_blend_epi8(mask_epi8, product_epi8, a);
  mask_epi8           = _mm512_cmpgt_epi8_mask(MIN_epi8, a);
  product_epi8        = _mm512_mask_blend_epi8(mask_epi8, product_epi8, a);

  return product_epi8;
}

} // namespace mm512
} // namespace srsran
