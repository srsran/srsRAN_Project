/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "packing_utils_generic.h"

using namespace srsran;
using namespace ofh;

void srsran::ofh::pack_bytes(bit_buffer& buffer, span<const int16_t> compressed_iq, unsigned iq_width)
{
  static constexpr unsigned NUM_BITS_IN_BYTE = units::bytes(1).to_bits().value();

  unsigned start_pos = 0;
  for (int16_t sample : compressed_iq) {
    unsigned left_to_pack = iq_width;

    while (left_to_pack != 0) {
      unsigned nbits  = std::min(NUM_BITS_IN_BYTE, left_to_pack);
      uint8_t  masked = static_cast<uint8_t>(sample & 0x00ff);
      if (left_to_pack > NUM_BITS_IN_BYTE) {
        masked = sample >> (left_to_pack - NUM_BITS_IN_BYTE);
        sample &= mask_lsb_ones<uint16_t>(left_to_pack - NUM_BITS_IN_BYTE);
      }
      buffer.insert(masked, start_pos, nbits);
      start_pos += nbits;
      left_to_pack -= nbits;
    }
  }

  // Clear tail bits of the last used byte.
  if (start_pos % NUM_BITS_IN_BYTE) {
    unsigned nbits = NUM_BITS_IN_BYTE - start_pos % NUM_BITS_IN_BYTE;
    buffer.insert(uint8_t(0), start_pos, nbits);
  }
}

int16_t srsran::ofh::unpack_bits(const bit_buffer_reader& buffer, unsigned offset, unsigned length)
{
  static constexpr unsigned NUM_BITS_IN_BYTE = units::bytes(1).to_bits().value();

  int16_t  value             = 0;
  unsigned bits_left_to_read = length;

  while (bits_left_to_read) {
    if (bits_left_to_read > NUM_BITS_IN_BYTE) {
      value = buffer.extract(offset, NUM_BITS_IN_BYTE);
      value <<= (bits_left_to_read - NUM_BITS_IN_BYTE);
      bits_left_to_read -= NUM_BITS_IN_BYTE;
      offset += NUM_BITS_IN_BYTE;
      continue;
    }

    value |= buffer.extract(offset, bits_left_to_read);
    bits_left_to_read = 0;
  }
  return value;
}
