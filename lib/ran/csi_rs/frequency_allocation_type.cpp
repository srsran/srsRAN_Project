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

#include "srsran/ran/csi_rs/frequency_allocation_type.h"

using namespace srsran;
using namespace csi_rs;

// Returns the scaling value applied to the bit position in the frequency allocation bitmap as per TS38.211
// Section 7.4.1.5.3.
static unsigned get_bitpos_scale(unsigned row)
{
  if (row == 1) {
    return 1;
  }
  if (row == 2) {
    return 1;
  }
  if (row == 4) {
    return 4;
  }
  return 2;
}

// Returns the number of bits in the frequency domain bitmap as per TS38.331 IE CSI-RS-ResourceMapping.
static unsigned get_bitmap_size(unsigned row)
{
  if (row == 1) {
    return 4;
  }
  if (row == 2) {
    return 12;
  }
  if (row == 4) {
    return 3;
  }
  return 6;
}

// Converts a frequency domain bitmap to the corresponding k_n values.
void srsran::csi_rs::convert_freq_domain(freq_allocation_index_type&      dst,
                                         const freq_allocation_mask_type& src,
                                         unsigned                         row)
{
  const unsigned scale = get_bitpos_scale(row);
  const unsigned size  = get_bitmap_size(row);
  for (unsigned i = size; i != 0; --i) {
    if (src.test(i - 1)) {
      dst.push_back(scale * (size - i));
    }
  }
}
