/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
