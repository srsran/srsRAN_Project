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

#include "ptrs_pdsch_generator_impl.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/ran/precoding/precoding_weight_matrix_formatters.h"
#include "srsran/ran/ptrs/ptrs_pattern.h"

using namespace srsran;

void ptrs_pdsch_generator_generic_impl::generate(resource_grid_writer& grid, const configuration& config)
{
  // Get the number of ports used for PT-RS: it is equal to the number of layers used for the PDSCH transmission.
  unsigned nof_ports = config.precoding.get_nof_layers();

  // Get the number of DM-RS per RB.
  unsigned nof_dmrs_prb = config.dmrs_config_type.nof_dmrs_per_rb();

  // The PT-RS antenna port is associated with the lowest indexed DM-RS antenna port among the DM-RS antenna ports
  // assigned for the PDSCH (TS38.214 Section 5.1.6.3).
  static constexpr unsigned i_layer = 0;

  // Prepare PT-RS pattern configuration.
  ptrs_pattern_configuration ptrs_pattern_config = {
      .rnti      = config.rnti,
      .dmrs_type = (config.dmrs_config_type == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2,
      .dmrs_symbol_mask = config.dmrs_symbols_mask,
      .rb_mask          = config.rb_mask,
      .time_allocation  = config.time_allocation,
      .freq_density     = config.freq_density,
      .time_density     = config.time_density,
      .re_offset        = config.re_offset,
      .nof_ports        = nof_ports,
  };

  // Generate pattern configuration.
  ptrs_pattern pattern = get_ptrs_pattern(ptrs_pattern_config);

  // Count number of RB containing PT-RS.
  unsigned nof_prb_ptrs = divide_ceil(pattern.rb_end - pattern.rb_begin, pattern.rb_stride);

  // Prepare temporary sequence.
  dmrs_sequence.resize(config.rb_mask.count() * nof_dmrs_prb);
  sequence.resize(1, nof_prb_ptrs);

  // Extract parameters to calculate the PRG initial state.
  unsigned nslot    = config.slot.slot_index();
  unsigned nidnscid = config.scrambling_id;
  unsigned nscid    = config.n_scid ? 1 : 0;
  unsigned nsymb    = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  unsigned l_0      = config.dmrs_symbols_mask.find_lowest();

  // Calculate initial sequence state.
  unsigned c_init = ((nsymb * nslot + l_0 + 1) * (2 * nidnscid + 1) * pow2(17) + (2 * nidnscid + nscid)) % pow2(31);

  // Generate sequence for all the symbols.
  pseudo_random_gen->init(c_init);
  pseudo_random_gen->advance(2 * (pattern.rb_begin - config.reference_point_k_rb) * nof_dmrs_prb);
  pseudo_random_gen->generate(dmrs_sequence, M_SQRT1_2 * config.amplitude);

  // Prepare the precoding for a single port.
  unsigned                prg_size     = config.precoding.get_prg_size();
  unsigned                nof_prg      = config.precoding.get_nof_prg();
  unsigned                nof_tx_ports = config.precoding.get_nof_ports();
  precoding_configuration port_precoding(1, config.precoding.get_nof_ports(), nof_prg, prg_size);
  for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
    for (unsigned i_tx_port = 0; i_tx_port != nof_tx_ports; ++i_tx_port) {
      // Extracts the port coefficient.
      cf_t port_coefficient = config.precoding.get_coefficient(i_layer, i_tx_port, i_prg);
      // Inserts the coefficient.
      port_precoding.set_coefficient(port_coefficient, 0, i_tx_port, i_prg);
    }
  }

  // Select samples from the sequence.
  span<cf_t> sequence_slice = sequence.get_slice(0);
  for (unsigned i_re = 0; i_re != nof_prb_ptrs; ++i_re) {
    sequence_slice[i_re] = dmrs_sequence[i_re * nof_dmrs_prb * pattern.rb_stride + pattern.re_offset[0] / 2];
  }

  // Iterate all the OFDM symbols of the transmission.
  for (unsigned i_symbol = config.time_allocation.start(), i_symbol_end = config.time_allocation.stop();
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // Skip if the OFDM symbol does not contain PT-RS.
    if (!pattern.symbol_mask.test(i_symbol)) {
      continue;
    }

    // Prepare RE mapping pattern for the symbol.
    re_pattern map_pattern;
    map_pattern.crb_mask.resize(config.rb_mask.size());
    map_pattern.symbols.resize(MAX_NSYMB_PER_SLOT);
    map_pattern.symbols.set(i_symbol);
    for (unsigned i_prb = pattern.rb_begin; i_prb < pattern.rb_end; i_prb += pattern.rb_stride) {
      map_pattern.crb_mask.set(i_prb);
    }
    map_pattern.re_mask.resize(NOF_SUBCARRIERS_PER_RB);
    map_pattern.re_mask.set(pattern.re_offset[0]);

    // Map sequence in the resource grid.
    mapper->map(grid, sequence, map_pattern, port_precoding);
  }
}
