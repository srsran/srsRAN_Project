/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief AVX512 support for LDPC.
///
/// Builds upon \ref avx2_support.h and specializes functions and templates for AVX512 registers.

#pragma once

#include "avx2_support.h"

namespace srsgnb {
namespace mm512 {

template <size_t N>
using avx512_array = detail::avx_array<detail::m512_wrapper, N>;

using avx512_span = detail::avx_span<detail::m512_wrapper>;

/// \brief Scales packed 8-bit integers in \c a by the scaling factor \c sf.
/// \param[in] a   Vector of packed 8-bit integers.
/// \param[in] sf  Scaling factor (0, 1].
/// \return    Vector of packed 8-bit integers with the scaling result.
inline __m512i scale_epi8(__m512i a, float sf)
{
  srsgnb_assert((sf > 0) && (sf <= 1), "Scaling factor out of range.");

  if (sf >= .9999) {
    return a;
  }

  static const __m512i mask_even_epi8 = _mm512_set1_epi16(0x00ff);
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
  return _mm512_xor_si512(p_even_epi16, p_odd_epi16);
}

} // namespace mm512
} // namespace srsgnb
