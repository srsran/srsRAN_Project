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
