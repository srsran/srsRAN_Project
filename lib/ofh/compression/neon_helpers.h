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
/// \brief NEON support for OFH Compression algorithms.

#pragma once

#include <algorithm>
#include <arm_neon.h>
#include <cstdint>

namespace srsran {
namespace ofh {
namespace neon {

/// \brief Determines BFP exponent for the resource block passed in a vector of three NEON registers.
///
/// The content of the three NEON registers is represented in the table below. Here REx stands for one component of
/// a unique resource element (16 bits long):
/// |       |         |         |           |         |
/// | ----- | ------- | ------- | --------- | ------- |
/// | \c reg0: | RE0 RE0 | RE1 RE1 | RE2 RE2   | RE3 RE3 |
/// | \c reg1: | RE4 RE4 | RE5 RE5 | RE6 RE6   | RE7 RE7 |
/// | \c reg2: | RE8 RE8 | RE9 RE9 | RE10 RE10 | RE11 RE11 |
///
/// \param[in] vec_s16x3  Vector of three NEON registers storing 16bit IQ pairs of the resource block.
/// \param[in] data_width Compressed samples bit width.
///
/// \return Calculated exponent value.
inline uint8_t determine_bfp_exponent(int16x8x3_t vec_s16x3, unsigned data_width)
{
  // Max allowed exponent (accounts for the extra bit used for sign).
  const uint16_t max_lzcnt   = (16 - data_width) + 1;
  uint16x4_t     vec_max_lzc = vdup_n_u16(max_lzcnt);

  // Calculate maximum value in the resource block (following instructions perform three pairwise comparisons between
  // the three registers and then find a maximum value across resulting vector).
  int16x8_t tmp_max_s16 = vmaxq_s16(vmaxq_s16(vec_s16x3.val[0], vec_s16x3.val[1]), vec_s16x3.val[2]);
  int16_t   max_s16     = vmaxvq_s16(tmp_max_s16);

  // Calculate minimum value in the resource block (following instructions perform three pairwise comparisons between
  // the three registers and then find a minimum value across resulting vector).
  int16x8_t tmp_min_s16 = vminq_s16(vminq_s16(vec_s16x3.val[0], vec_s16x3.val[1]), vec_s16x3.val[2]);
  int16_t   min_s16     = vminvq_s16(tmp_min_s16);

  // Calculate maximum absolute value.
  uint16_t max_abs = std::max(std::abs(max_s16), std::abs(min_s16) - 1);

  // Calculate number of leading zeros.
  uint16x4_t lzc_u16 = vclz_u16(vdup_n_u16(max_abs));

  // Calculate exponent.
  return static_cast<uint8_t>(vget_lane_u16(vsub_u16(vec_max_lzc, lzc_u16), 0));
}

} // namespace neon
} // namespace ofh
} // namespace srsran
