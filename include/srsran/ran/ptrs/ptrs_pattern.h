/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/interval.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/ptrs/ptrs.h"
#include "srsran/ran/ptrs/ptrs_constants.h"
#include "srsran/ran/resource_allocation/rb_bitmap.h"
#include "srsran/ran/resource_block.h"
#include "srsran/ran/rnti.h"

namespace srsran {

/// Required parameters to compute a PT-RS allocation pattern in a resource grid.
struct ptrs_pattern_configuration {
  /// RNTI associated with the DCI scheduling the transmission, parameter \f$n_{RNTI}\f$.
  rnti_t rnti;
  /// DM-RS config type (\e dmrsConfigType).
  dmrs_config_type dmrs_type;
  /// DM-RS position mask. Indicates the OFDM symbols carrying DM-RS within the slot.
  bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_symbol_mask;
  /// Frequency domain allocation as CRB mask. The entries set to true are used for transmission.
  crb_bitmap rb_mask;
  /// Time domain allocation of the transmission containing PT-RS.
  interval<uint8_t> time_allocation;
  /// Frequency domain density.
  ptrs_frequency_density freq_density;
  /// Time domain density.
  ptrs_time_density time_density;
  /// Resource element offset.
  ptrs_re_offset re_offset;
  /// Number of transmit ports.
  unsigned nof_ports;
};

/// PT-RS allocation pattern in a resource grid.
struct ptrs_pattern {
  /// Resource block where the pattern begins in frequency domain, the range is {0, ..., 274}.
  unsigned rb_begin;
  /// Resource block where the pattern ends in frequency domain (excluded), the range is {1, ..., 275}.
  unsigned rb_end;
  /// Resource block index jump.
  unsigned rb_stride;
  /// RE used within the RB that contains PT-RS for a certain port.
  static_vector<uint8_t, ptrs_constants::max_nof_ports> re_offset;
  /// Symbol mask. True entries indicate the symbols affected by the pattern within the slot.
  bounded_bitset<MAX_NSYMB_PER_SLOT> symbol_mask;
};

/// Generates the PT-RS pattern from the given PT-RS parameters.
ptrs_pattern get_ptrs_pattern(const ptrs_pattern_configuration& pattern_config);

} // namespace srsran
