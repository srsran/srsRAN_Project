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

#include "srsran/ran/ptrs/ptrs_pattern.h"

using namespace srsran;

static span<const uint8_t> get_k_re_ref(dmrs_config_type dmrs_type, ptrs_re_offset offset, unsigned nof_ports)
{
  // TS38.211 Table 7.4.1.2.2-1 for DM-RS Configuration type 1.
  static constexpr std::array<std::array<uint8_t, 4>, 4> k_re_ref_type1 = {
      {{0, 2, 1, 3}, {2, 4, 3, 5}, {6, 8, 7, 9}, {8, 10, 9, 11}}};

  // TS38.211 Table 7.4.1.2.2-1 for DM-RS Configuration type 2.
  static constexpr std::array<std::array<uint8_t, 6>, 4> k_re_ref_type2 = {
      {{0, 1, 2, 3, 4, 5}, {1, 6, 3, 8, 5, 10}, {6, 7, 8, 9, 10, 11}, {7, 0, 9, 2, 11, 4}}};

  // Convert RE offset to a value.
  unsigned re_offset = to_value(offset);

  // Select the correct subcarrier reference table depending on the DM-RS for PDSCH type and RE offset parameter.
  span<const uint8_t> k_re_ref = (dmrs_type == dmrs_config_type::type1)
                                     ? span<const uint8_t>(k_re_ref_type1[re_offset])
                                     : span<const uint8_t>(k_re_ref_type2[re_offset]);

  interval<unsigned, true> nof_ports_range(1, k_re_ref.size());
  srsran_assert(nof_ports_range.contains(nof_ports),
                "The number of ports (i.e., {}), is out of the range {} for DM-RS type {}.",
                nof_ports,
                nof_ports_range,
                static_cast<unsigned>(dmrs_type));

  return k_re_ref.first(nof_ports);
}

ptrs_pattern srsran::get_ptrs_pattern(const ptrs_pattern_configuration& pattern_config)
{
  srsran_assert(pattern_config.rb_mask.is_contiguous(), "Only contiguous allocations are supported.");
  int rb_begin = pattern_config.rb_mask.find_lowest();
  int rb_end   = pattern_config.rb_mask.find_highest();
  srsran_assert((rb_begin >= 0) && (rb_begin <= rb_end), "Invalid RB mask (i.e., {}).", pattern_config.rb_mask);

  // Count number of RB used for the transmission, parameter N_{RB}.
  unsigned N_rb = pattern_config.rb_mask.count();

  // Obtain frequency density, parameter K_{PT-RS}.
  unsigned K_ptrs = to_value(pattern_config.freq_density);

  // Obtain time density, parameter L_{PT-RS}.
  unsigned L_ptrs = to_value(pattern_config.time_density);

  // Compute the reference resource block.
  unsigned k_rb_ref = to_value(pattern_config.rnti) % K_ptrs;
  if (N_rb % K_ptrs != 0) {
    k_rb_ref = to_value(pattern_config.rnti) % (N_rb % K_ptrs);
  }

  // Get the RE offsets for each port.
  span<const uint8_t> k_re_ref =
      get_k_re_ref(pattern_config.dmrs_type, pattern_config.re_offset, pattern_config.nof_ports);

  // Creates time domain allocation.
  bounded_bitset<MAX_NSYMB_PER_SLOT> symbol_mask(pattern_config.dmrs_symbol_mask.size());

  // Fill the PT-RS time domain allocation.
  // Step 1.
  int i = 0, l_ref = pattern_config.time_allocation.start();

  // Iterate Step 2.
  while (l_ref + i * L_ptrs < pattern_config.time_allocation.stop()) {
    // Check for an interval if it contains DM-RS
    unsigned startpos = std::max(l_ref + (i - 1) * static_cast<int>(L_ptrs) + 1, l_ref);
    unsigned endpos   = l_ref + i * L_ptrs;
    int      dmrs_pos = pattern_config.dmrs_symbol_mask.find_highest(startpos, endpos + 1);

    // If it contains DM-RS.
    if (dmrs_pos >= 0) {
      i     = 1;
      l_ref = dmrs_pos;
      continue;
    }

    // Otherwise it is a valid symbol for PT-RS.
    symbol_mask.set(l_ref + i * L_ptrs);
    ++i;
  }

  // Return pattern.
  return ptrs_pattern{.rb_begin    = k_rb_ref + rb_begin,
                      .rb_end      = rb_begin + N_rb,
                      .rb_stride   = K_ptrs,
                      .re_offset   = {k_re_ref.begin(), k_re_ref.end()},
                      .symbol_mask = symbol_mask};
}
