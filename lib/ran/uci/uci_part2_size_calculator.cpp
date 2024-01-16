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

#include "srsran/ran/uci/uci_part2_size_calculator.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

static unsigned extract_parameter(unsigned offset, unsigned width, const uci_payload_type& payload)
{
  // Verify the payload read range is valid.
  srsran_assert(offset + width <= payload.size(),
                "The offset (i.e., {}) plus the width (i.e., {}) exceeds the payload size (i.e., {}).",
                offset,
                width,
                payload.size());

  // Ignore for zero width.
  if (width == 0) {
    return 0;
  }

  // Get slice of the CSI Part 1.
  uci_payload_type slice = payload.slice(offset, offset + width);

  // Extract parameter value.
  uint64_t param = slice.to_uint64();

  // Reverse parameter bits.
  param = bit_reverse(param) >> (64 - width);

  return static_cast<unsigned>(param);
}

unsigned srsran::uci_part2_get_size(const uci_payload_type& part1, const uci_part2_size_description& descr)
{
  unsigned result = 0;

  // Iterate all entries.
  for (const uci_part2_size_description::entry& entry : descr.entries) {
    unsigned index          = 0;
    unsigned index_bitwidth = 0;

    for (const uci_part2_size_description::parameter& parameter : entry.parameters) {
      // Extract the value of the parameter.
      unsigned value = extract_parameter(parameter.offset, parameter.width, part1);

      // Combine the parameter value with the current index.
      index = (index << parameter.width) | value;

      // Accumulate the index bit width.
      index_bitwidth += parameter.width;
    }

    // Verify the map size is according to the index bit width.
    srsran_assert(entry.map.size() == (1U << index_bitwidth),
                  "Invalid map size (i.e., {}), expected {} entries.",
                  entry.map.size(),
                  (1U << index_bitwidth));

    // Verify that the index is within the map size.
    srsran_assert(
        index < entry.map.size(), "Index value (i.e., {}) exceeds the map size (i.e., {}).", index, entry.map.size());

    // Add the Part 2 size corresponding to this entry.
    result += entry.map[index];
  }

  return result;
}
