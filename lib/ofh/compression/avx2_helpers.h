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
/// \brief AVX2 support for OFH Compression algorithms.

#pragma once

#include "srsran/adt/span.h"
#include <immintrin.h>

namespace srsran {
namespace ofh {
namespace mm256 {

/// Type of AVX2 intrinsic calculating min or max values between two vectors.
using _mm256_comparator = __m256i (*)(__m256i a, __m256i b);

/// \brief Finds the maximum (or minimum) 16bit sample in each of the two input resource blocks, returns its absolute
/// value.
///
/// Finds absolute maximum (or minimum, depending on the comparator passed as the template argument) values
/// for each of the two RBs passed in a three AVX2 registers.
///
/// Registers are evenly divided between two RBs so that IQ samples of each RB occupy one 128bit lane.
/// The following diagram shows the input format. Here RBx stands for one unique RE (a pair of IQ samples, 32 bits long)
/// pertaining to a respective RB:
/// |       |         |         |         |         |
/// | ----- | ------- | ------- | ------- | ------- |
/// | \c v0_epi16: | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 |
/// | \c v1_epi16: | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 |
/// | \c v2_epi16: | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 |
///
/// The function first performs vertical min/max search across the three registers (comparing 16bit samples), then
/// horizontal min/max across elements in each 128bit lane of the resulting vector. Finally, the function takes the
/// absolute value of the two 16bit samples computed thus far and stores them in the output span.
///
/// \tparam    COMPARE  AVX2 intrinsic used for comparison, either \c mm256_min_epi16 or \c _mm256_max_epi16.
/// \param[out] abs     A span of two absolute values of max (or min) samples in each of the two input RBs.
/// \param[in] v0_epi16 AVX2 register storing first four IQ pairs of each of the two RB.
/// \param[in] v1_epi16 AVX2 register storing second four IQ pairs of each of the two RB.
/// \param[in] v2_epi16 AVX2 register storing last four IQ pairs of each of the two RB.
template <_mm256_comparator COMPARE>
inline void find_rbs_abs_min_max_values(span<uint16_t> abs, __m256i v0_epi16, __m256i v1_epi16, __m256i v2_epi16)
{
  // Find vertical min/max across every 3 16bit values in each resource block.
  __m256i cmp_8val_epi16 = COMPARE(COMPARE(v0_epi16, v1_epi16), v2_epi16);
  // Now 16bit vector elements look as follows:
  // [max(I00, I08, I04), max(Q00, Q08, Q04), max(I01, I09, I05), max(Q01, Q09, Q05), ...].

  // Swap 64bit words inside 128bit lanes and find 4 min/max values.
  __m256i cmp_8val_swp_epi16 = _mm256_shuffle_epi32(cmp_8val_epi16, 0x4e);
  __m256i cmp_4val_epi16     = COMPARE(cmp_8val_epi16, cmp_8val_swp_epi16);
  // Swap 32bit words and find 2 min/max values.
  __m256i cmp_4val_swp_epi16 = _mm256_shuffle_epi32(cmp_4val_epi16, 0x1b);
  __m256i cmp_2val_epi16     = COMPARE(cmp_4val_epi16, cmp_4val_swp_epi16);
  // Swap 16bit words and find final min/max values for each RB.
  const __m256i shuffle_mask = _mm256_setr_epi8(
      2, 3, 0, 1, 6, 7, 4, 5, 8, 9, 10, 11, 14, 15, 12, 13, 2, 3, 0, 1, 6, 7, 4, 5, 8, 9, 10, 11, 14, 15, 12, 13);
  __m256i cmp_2val_swp_epi16 = _mm256_shuffle_epi8(cmp_2val_epi16, shuffle_mask);
  __m256i cmp_rb0_rb1_epi16  = COMPARE(cmp_2val_epi16, cmp_2val_swp_epi16);
  __m256i cmp_rb0_rb1_epu16  = _mm256_abs_epi16(cmp_rb0_rb1_epi16);

  // Extract absolute minimum/maximum values for each RB.
  abs[0] = _mm256_extract_epi16(cmp_rb0_rb1_epu16, 0);
  abs[1] = _mm256_extract_epi16(cmp_rb0_rb1_epu16, 8);
}

/// \brief Finds maximum absolute value across 16bit IQ samples in each of the two input resource blocks passed in three
/// AVX2 registers.
///
/// The content of the three input AVX2 registers is represented in the table below. Here RBx means one unique RE
/// (pair of IQ samples, 32 bits long) pertaining to a respective RB:
/// |       |         |         |         |         |
/// | ----- | ------- | ------- | ------- | ------- |
/// | \c rb0_epi16:  | RB0 RB0 | RB0 RB0 | RB0 RB0 | RB0 RB0 |
/// | \c rb01_epi16: | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 |
/// | \c rb1_epi16:  | RB1 RB1 | RB1 RB1 | RB1 RB1 | RB1 RB1 |
///
/// \param[out] max_abs   A span of two maximum absolute values in the two input RBs.
/// \param[in] rb0_epi16  AVX2 register storing 16bit IQ pairs of the first RB.
/// \param[in] rb01_epi16 AVX2 register storing 16bit IQ pairs of the first and second RBs.
/// \param[in] rb1_epi16  AVX2 register storing 16bit IQ pairs of the second RB.
inline void calculate_max_abs(span<unsigned> max_abs, __m256i rb0_epi16, __m256i rb01_epi16, __m256i rb1_epi16)
{
  std::array<uint16_t, 2> abs_min_values;
  std::array<uint16_t, 2> abs_max_values;

  // Reorganize vectors to be able to vertically compare 16bit samples pertaining to the same resource block:
  // [ RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 ]
  // [ RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 ]
  // [ RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 ]
  __m256i v0_epi16 = _mm256_permute2f128_si256(rb0_epi16, rb1_epi16, 0x20);
  __m256i v2_epi16 = _mm256_permute2f128_si256(rb0_epi16, rb1_epi16, 0x31);
  __m256i v1_epi16 = rb01_epi16;

  find_rbs_abs_min_max_values<_mm256_min_epi16>(abs_min_values, v0_epi16, v1_epi16, v2_epi16);
  find_rbs_abs_min_max_values<_mm256_max_epi16>(abs_max_values, v0_epi16, v1_epi16, v2_epi16);

  max_abs[0] = std::max<unsigned>(abs_max_values[0], abs_min_values[0] - 1U);
  max_abs[1] = std::max<unsigned>(abs_max_values[1], abs_min_values[1] - 1U);
}

} // namespace mm256
} // namespace ofh
} // namespace srsran
