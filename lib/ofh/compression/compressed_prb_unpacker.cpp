/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "compressed_prb_unpacker.h"
#include "srsran/support/units.h"

using namespace srsran;
using namespace ofh;

int16_t compressed_prb_unpacker::unpack(unsigned offset, unsigned length) const
{
  static constexpr unsigned NUM_BITS_IN_BYTE = units::bytes(1).to_bits().value();

  int16_t  value             = 0;
  unsigned bits_left_to_read = length;

  while (bits_left_to_read) {
    if (bits_left_to_read > NUM_BITS_IN_BYTE) {
      value = prb.get_bit_buffer().extract(offset, NUM_BITS_IN_BYTE);
      value <<= (bits_left_to_read - NUM_BITS_IN_BYTE);
      bits_left_to_read -= NUM_BITS_IN_BYTE;
      offset += NUM_BITS_IN_BYTE;
      continue;
    }

    value |= prb.get_bit_buffer().extract(offset, bits_left_to_read);
    bits_left_to_read = 0;
  }

  return value;
}
