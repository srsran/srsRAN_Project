/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
/// \brief AVX512 support for OFH Compression algorithms.

#pragma once

#include <immintrin.h>

namespace srsran {
namespace ofh {
namespace mm512 {

/// Type of AVX512 intrinsic calculating min or max values between two vectors.
using _mm512_comparator = __m512i (*)(__m512i a, __m512i b);

/// \brief Finds the maximum (or minimum) 16bit sample in each of the four input resource blocks, returns its absolute
/// value.
///
/// Finds absolute maximum (or minimum, depending on the comparator passed as the template argument) values for each of
/// the four RBs passed in three AVX512 registers.
///
/// AVX512 register bits are evenly divided between four RBs so that IQ samples of each RB occupy one 128bit lane.
/// The following diagram shows the input format. Here RBx stands for one unique RE (a pair of IQ samples, 32 bits long)
/// pertaining to a respective RB:
/// |       |         |         |         |         |         |         |         |         |
/// | ----- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- |
/// | \c v0_epi16: | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 | RB2 RB2 | RB2 RB2 | RB3 RB3 | RB3 RB3 |
/// | \c v1_epi16: | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 | RB2 RB2 | RB2 RB2 | RB3 RB3 | RB3 RB3 |
/// | \c v2_epi16: | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 | RB2 RB2 | RB2 RB2 | RB3 RB3 | RB3 RB3 |
///
/// The function first performs vertical min/max search across the three registers (comparing 16bit samples), then
/// horizontal min/max across elements in each 128bit lane of the resulting vector. Finally, the function takes the
/// absolute value of the four 16bit samples computed thus far.
///
/// \tparam COMPARE     AVX512 intrinsic used for comparison, either \c _mm512_min_epi16 or \c _mm512_max_epi16.
/// \param[in] v0_epi16 AVX512 register storing first four IQ pairs of each of the four RB.
/// \param[in] v1_epi16 AVX512 register storing second four IQ pairs of each of the four RB.
/// \param[in] v2_epi16 AVX512 register storing last four IQ pairs of each of the four RB.
///
/// \return An AVX512 register storing the result for each of the four RBs in every 128bit lane.
template <_mm512_comparator COMPARE>
inline __m512i find_abs_min_max_values(__m512i v0_epi16, __m512i v1_epi16, __m512i v2_epi16)
{
  // Find vertical min/max across every 8 16bit values pertaining to the same PRB.
  __m512i cmp_8val_epi16 = COMPARE(COMPARE(v0_epi16, v1_epi16), v2_epi16);

  // Swap 64bit words inside 128bit lanes and find 4 min/max values.
  __m512i cmp_8val_swp_epi16 =
      _mm512_mask_shuffle_epi32(_mm512_set1_epi64(0), 0xffff, cmp_8val_epi16, (_MM_PERM_ENUM)_MM_SHUFFLE(1, 0, 3, 2));
  __m512i cmp_4val_epi16 = COMPARE(cmp_8val_epi16, cmp_8val_swp_epi16);

  // Swap 32bit words and find 2 min/max values.
  __m512i cmp_4val_swp_epi16 =
      _mm512_mask_shuffle_epi32(_mm512_set1_epi64(0), 0xffff, cmp_4val_epi16, (_MM_PERM_ENUM)_MM_SHUFFLE(2, 3, 0, 1));
  __m512i cmp_2val_epi16 = COMPARE(cmp_4val_epi16, cmp_4val_swp_epi16);

  // Swap 16bit words and find final min/max values for each PRB.
  const __m512i shuffle_mask = _mm512_setr_epi64(0x0504070601000302,
                                                 0x0d0c0f0e09080b0a,
                                                 0x0504070601000302,
                                                 0x0d0c0f0e09080b0a,
                                                 0x0504070601000302,
                                                 0x0d0c0f0e09080b0a,
                                                 0x0504070601000302,
                                                 0x0d0c0f0e09080b0a);

  __m512i cmp_2val_swp_epi16 = _mm512_shuffle_epi8(cmp_2val_epi16, shuffle_mask);
  return _mm512_abs_epi16(COMPARE(cmp_2val_epi16, cmp_2val_swp_epi16));
}

/// \brief Finds maximum absolute value across 16bit IQ samples in each of the four input resource blocks passed in
/// three AVX512 registers.
///
/// The content of the three input AVX512 registers is represented in the table below. Here RBx stands for one unique RE
/// (pair of IQ samples, 32 bits long) pertaining to a respective RB:
/// |       |         |         |         |         |         |         |         |         |
/// | ----- | ------- | ------- | ------- | ------- | ------- | ------- | ------- | ------- |
/// | \c r0_epi16: | RB0 RB0 | RB0 RB0 | RB0 RB0 | RB0 RB0 | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 |
/// | \c r1_epi16: | RB1 RB1 | RB1 RB1 | RB1 RB1 | RB1 RB1 | RB2 RB2 | RB2 RB2 | RB2 RB2 | RB2 RB2 |
/// | \c r2_epi16: | RB2 RB2 | RB2 RB2 | RB3 RB3 | RB3 RB3 | RB3 RB3 | RB3 RB3 | RB3 RB3 | RB3 RB3 |
///
/// \param[in] r0_epi16 AVX512 register storing 16bit IQ pairs of the first and second RBs.
/// \param[in] r1_epi16 AVX512 register storing 16bit IQ pairs of the second and third RBs.
/// \param[in] r2_epi16 AVX512 register storing 16bit IQ pairs of the third and fourth RBs.
/// \return An AVX512 register storing the result for each of the four RBs in every 128bit lane.
inline __m512i calculate_max_abs(__m512i r0_epi16, __m512i r1_epi16, __m512i r2_epi16)
{
  const __m512i ONE_EPI16 = _mm512_set1_epi16(1);

  // Permute input vectors so that the pairwise comparison can be used. The resulting vectors look as follows:
  // [ RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 | RB2 RB2 | RB2 RB2 | RB3 RB3 | RB3 RB3 ]
  // [ RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 | RB2 RB2 | RB2 RB2 | RB3 RB3 | RB3 RB3 ]
  // [ RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 | RB2 RB2 | RB2 RB2 | RB3 RB3 | RB3 RB3 ]
  __m512i v0_epi16 =
      _mm512_mask_shuffle_i64x2(r0_epi16, 0b11111100, r1_epi16, r2_epi16, (_MM_PERM_ENUM)_MM_SHUFFLE(1, 0, 0, 0));
  __m512i v1_epi16 =
      _mm512_mask_shuffle_i64x2(r1_epi16, 0b11000011, r0_epi16, r2_epi16, (_MM_PERM_ENUM)_MM_SHUFFLE(2, 0, 0, 1));
  __m512i v2_epi16 =
      _mm512_mask_shuffle_i64x2(r2_epi16, 0b00111111, r0_epi16, r1_epi16, (_MM_PERM_ENUM)_MM_SHUFFLE(0, 3, 3, 2));

  __m512i abs_min_epu16 = find_abs_min_max_values<_mm512_min_epi16>(v0_epi16, v1_epi16, v2_epi16);
  __m512i abs_max_epu16 = find_abs_min_max_values<_mm512_max_epi16>(v0_epi16, v1_epi16, v2_epi16);

  // Subtract 1 from absolute minimum value.
  abs_min_epu16 = _mm512_subs_epu16(abs_min_epu16, ONE_EPI16);

  return _mm512_max_epu16(abs_min_epu16, abs_max_epu16);
}
/// \brief Determines BFP exponent for each of the four input RBs passed in three AVX512 registers.
///
/// \param[in] r0_epi16   AVX512 register storing 16bit IQ pairs of the first and second RBs.
/// \param[in] r1_epi16   AVX512 register storing 16bit IQ pairs of the second and third RBs.
/// \param[in] r2_epi16   AVX512 register storing 16bit IQ pairs of the third and fourth RBs.
/// \param[in] data_width Compressed samples bit width.
///
/// \return Four exponents in the LSB bits of every 128bit lane.
inline __m512i determine_bfp_exponent(__m512i r0_epi16, __m512i r1_epi16, __m512i r2_epi16, unsigned data_width)
{
  // Max allowed exponent (account for the extra 16 MSBs and a sign bit counted by _mm512_lzcnt_epi32 intrinsic).
  unsigned max_lzcnt     = (16 - data_width) + 16 + 1;
  __m512i  max_exp_epu32 = _mm512_set1_epi32(max_lzcnt);

  // Calculate maximum absolute values in the four PRBs.
  __m512i max_abs_epu16 = calculate_max_abs(r0_epi16, r1_epi16, r2_epi16);

  // Mask out 16 MSB bits in each 32bit word.
  max_abs_epu16 = _mm512_and_epi32(max_abs_epu16, _mm512_set1_epi32(0x0000ffff));

  // Compute BFP exponent as a number of leading zeros.
  __m512i lz_count_epi32 = _mm512_lzcnt_epi32(max_abs_epu16);

  // Subtract with saturation.
  return _mm512_subs_epu16(max_exp_epu32, lz_count_epi32);
}

} // namespace mm512
} // namespace ofh
} // namespace srsran
