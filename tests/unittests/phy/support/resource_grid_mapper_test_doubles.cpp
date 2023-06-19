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

#include "resource_grid_mapper_test_doubles.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"

using namespace srsran;

void resource_grid_mapper_spy::map(const re_buffer_reader&        input,
                                   const re_pattern_list&         pattern,
                                   const re_pattern_list&         reserved,
                                   const precoding_configuration& precoding)
{
  srsran_assert(precoding.get_nof_prg() == 1, "Number of PRG must be one.");

  unsigned i_re = 0;
  for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    // Get the symbol RE mask.
    bounded_bitset<MAX_RB * NRE> symbol_re_mask(MAX_RB * NRE);
    pattern.get_inclusion_mask(symbol_re_mask, i_symbol);
    reserved.get_exclusion_mask(symbol_re_mask, i_symbol);

    // Find the highest used subcarrier. Skip symbol if no active subcarrier.
    int i_highest_subc = symbol_re_mask.find_highest();
    if (i_highest_subc < 0) {
      continue;
    }

    // Resize the mask to the highest subcarrier, ceiling to PRB.
    symbol_re_mask.resize(divide_ceil(i_highest_subc + 1, NRE) * NRE);

    // Count the number of mapped RE.
    unsigned nof_re = symbol_re_mask.count();

    for (unsigned i_layer = 0, nof_layers = input.get_nof_slices(); i_layer != nof_layers; ++i_layer) {
      // Map each layer without precoding.
      span<const cf_t> layer_data = input.get_slice(i_layer);

      unsigned i_layer_port = 0;
      cf_t     coefficient  = 0;
      for (unsigned i_port = 0, i_port_end = precoding.get_nof_ports(); i_port != i_port_end; ++i_port) {
        cf_t coefficient_port = precoding.get_coefficient(i_layer, i_port, 0);

        // The precoding coefficient is not zero.
        if (coefficient_port != cf_t()) {
          if (coefficient != cf_t()) {
            report_fatal_error("Invalid precoding configuration {}.", precoding);
          }

          coefficient  = coefficient_port;
          i_layer_port = i_port;
        }
      }

      span<const cf_t> unmapped =
          rg_writer_spy.put(i_layer_port, i_symbol, 0, symbol_re_mask, layer_data.subspan(i_re, nof_re));
      srsran_assert(unmapped.empty(), "Not all REs have been mapped to the grid. {} remaining.", unmapped.size());
    }

    // Advance RE counter.
    i_re += nof_re;
  }

  srsran_assert(i_re == input.get_nof_re(),
                "The nuber of mapped RE (i.e., {}) does not match the number of input RE (i.e., {}).",
                i_re,
                input.get_nof_re());
}

void resource_grid_mapper_spy::map(const re_buffer_reader&        input,
                                   const re_pattern_list&         pattern,
                                   const precoding_configuration& precoding)
{
  // Map with an empty list of reserved RE patterns.
  map(input, pattern, re_pattern_list(), precoding);
}
