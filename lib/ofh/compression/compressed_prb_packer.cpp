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

#include "compressed_prb_packer.h"
#include "srsran/support/units.h"

using namespace srsran;
using namespace ofh;

void compressed_prb_packer::pack(span<const int16_t> compressed_iq, unsigned iq_width)
{
  srsran_assert(compressed_iq.size() == NOF_SUBCARRIERS_PER_RB * 2, "Compressed data must contain 12 IQ samples");
  srsran_assert(iq_width <= MAX_IQ_WIDTH, "Requested width can not exceed 16");
  static constexpr unsigned NUM_BITS_IN_BYTE = units::bytes(1).to_bits().value();

  auto& bit_buffer = prb.get_bit_buffer();

  unsigned start_pos = 0;
  for (int16_t sample : compressed_iq) {
    unsigned left_to_pack = iq_width;

    while (left_to_pack != 0) {
      unsigned nbits  = std::min(NUM_BITS_IN_BYTE, left_to_pack);
      uint8_t  masked = sample;
      if (left_to_pack > NUM_BITS_IN_BYTE) {
        masked = sample >> (left_to_pack - NUM_BITS_IN_BYTE);
        sample &= mask_lsb_ones<uint16_t>(left_to_pack - NUM_BITS_IN_BYTE);
      }
      bit_buffer.insert(masked, start_pos, nbits);
      start_pos += nbits;
      left_to_pack -= nbits;
    }
  }

  // Clear tail bits of the last used byte.
  if (start_pos % NUM_BITS_IN_BYTE) {
    unsigned nbits = NUM_BITS_IN_BYTE - start_pos % NUM_BITS_IN_BYTE;
    bit_buffer.insert(uint8_t(0), start_pos, nbits);
  }

  prb.set_stored_size((compressed_iq.size() * iq_width / NUM_BITS_IN_BYTE) +
                      ((compressed_iq.size() * iq_width % NUM_BITS_IN_BYTE) ? 1U : 0));
}
