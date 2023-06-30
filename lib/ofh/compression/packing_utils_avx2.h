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

#pragma once

#include "srsran/ofh/compression/compressed_prb.h"
#include <immintrin.h>

namespace srsran {
namespace ofh {
namespace mm256 {

/// \brief Packs 16bit IQ values of the two RBs using the specified width and big-endian format.
///
/// The following diagram shows the input format. Here RBx stands for one unique RE (pair of IQ samples, 32 bits long)
/// pertaining to a respective RB):
/// |       |         |         |         |         |
/// | ----- | ------- | ------- | ------- | ------- |
/// | \c r0: | RB0 RB0 | RB0 RB0 | RB0 RB0 | RB0 RB0 |
/// | \c r1: | RB0 RB0 | RB0 RB0 | RB1 RB1 | RB1 RB1 |
/// | \c r2: | RB1 RB1 | RB1 RB1 | RB1 RB1 | RB1 RB1 |
///
/// \param[out] c_prbs Span of two output compressed PRBs storing packed bytes.
/// \param[in] r0      AVX2 register storing 16bit IQ pairs of the first PRB.
/// \param[in] r1      AVX2 register storing 16bit IQ pairs of the first and second PRB.
/// \param[in] r2      AVX2 register storing 16bit IQ pairs of the second PRB.
/// \param[in] iq_width Bit width of the resulting packed IQ samples.
void pack_prbs_big_endian(span<ofh::compressed_prb> c_prbs, __m256i r0, __m256i r1, __m256i r2, unsigned iq_width);

/// \brief Unpacks packed IQ samples stored as bytes in big-endian format to an array of 16bit signed values.
///
/// \param[out] unpacked_iq_data A span of 32 16bit integers, corresponding to \c NOF_CARRIERS_PER_RB unpacked IQ pairs.
/// \param[in]  packed_data      A span of input packed bytes.
/// \param[in]  iq_width         Bit width of the packed IQ samples.
///
/// \note The \c unpacked_iq_data parameter should be sized to store 32 output IQ samples: it is 24 IQ samples of one RB
/// rounded up to 32-byte boundary required by AVX2 intrinsics.
void unpack_prb_big_endian(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data, unsigned iq_width);

/// \brief Checks whether the requested bit width is supported by the AVX2 implementation.
/// \param[in] iq_width Requested bit width.
///
/// \return True in case packing/unpacking with the requested bit width is supported
bool iq_width_packing_supported(unsigned iq_width);

} // namespace mm256
} // namespace ofh
} // namespace srsran
