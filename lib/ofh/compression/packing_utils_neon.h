/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "neon_helpers.h"
#include "srsran/ofh/compression/compressed_prb.h"

namespace srsran {
namespace ofh {
namespace neon {

/// \brief Packs 16bit IQ values of a resource block using the specified width and big-endian format.
///
/// \param[out] c_prb   Output PRB storing compressed packed bytes.
/// \param[in]  reg     Vector of three NEON registers storing 16bit IQ pairs of the PRB.
/// \param[in] iq_width Bit width of the resulting packed IQ samples.
void pack_prb_big_endian(ofh::compressed_prb& c_prb, int16x8x3_t regs, unsigned iq_width);

/// \brief Unpacks packed IQ samples stored as bytes in big-endian format to an array of 16bit signed values.
///
/// \param[out] unpacked_iq_data A sequence of 24 integers, corresponding to \c NOF_CARRIERS_PER_RB unpacked IQ pairs.
/// \param[in]  packed_data      A sequence of input packed bytes.
/// \param[in] iq_width          Bit width of the packed IQ samples.
void unpack_prb_big_endian(span<int16_t> unpacked_iq_data, span<const uint8_t> packed_data, unsigned iq_width);

/// \brief Checks whether the requested bit width is supported by the NEON implementation.
/// \param[in] iq_width Requested bit width.
///
/// \return True in case packing/unpacking with the requested bit width is supported.
bool iq_width_packing_supported(unsigned iq_width);

} // namespace neon
} // namespace ofh
} // namespace srsran
