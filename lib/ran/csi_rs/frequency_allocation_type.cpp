/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

// Converts a frequency domain bitmap to the corresponding k_n values.
void srsran::csi_rs::convert_freq_domain(freq_allocation_index_type&      dst,
                                         const freq_allocation_mask_type& src,
                                         unsigned                         row)
{
  const unsigned scale = get_bitpos_scale(row);
  for (const auto pos : src.get_bit_positions()) {
    dst.push_back(scale * pos);
  }
}
