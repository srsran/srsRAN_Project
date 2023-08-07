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

#include "srsran/ran/uci/uci_part2_size_calculator.h"
#include "srsran/srsvec/bit.h"

using namespace srsran;

unsigned srsran::uci_part2_get_size(span<const uint8_t> part1, const uci_part2_size_description& descr)
{
  unsigned result = 0;

  // Iterate all entries.
  for (const uci_part2_size_description::entry& entry : descr.entries) {
    unsigned index          = 0;
    unsigned index_bitwidth = 0;

    for (const uci_part2_size_description::parameter& parameter : entry.parameters) {
      // Extract the value of the parameter.
      unsigned value = srsvec::bit_pack(part1.subspan(parameter.offset, parameter.width));

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
