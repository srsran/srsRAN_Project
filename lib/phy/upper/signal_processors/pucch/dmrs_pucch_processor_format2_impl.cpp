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

#include "dmrs_pucch_processor_format2_impl.h"
#include "srsran/srsvec/prod.h"

using namespace srsran;

/// \brief DM-RS RE allocation pattern for PUCCH Format 2.
///
/// Indicates the Resource Elements containing DM-RS symbols within a PRB, as per TS38.211 Section 6.4.1.3.2.2.
static const bounded_bitset<NRE> format2_prb_re_mask =
    {false, true, false, false, true, false, false, true, false, false, true, false};

unsigned dmrs_pucch_processor_format2_impl::c_init(unsigned symbol, const dmrs_pucch_processor::config_t& config)
{
  uint64_t n_slot = config.slot.slot_index();
  uint64_t n_id   = config.n_id_0;

  return ((get_nsymb_per_slot(config.cp) * n_slot + symbol + 1) * (2 * n_id + 1) * pow2(17) + 2 * n_id) % pow2(31);
}

void dmrs_pucch_processor_format2_impl::generate_sequence(span<cf_t>                            sequence,
                                                          unsigned                              symbol,
                                                          unsigned                              starting_prb,
                                                          const dmrs_pucch_processor::config_t& config)
{
  // Initialize pseudo-random generator.
  prg->init(c_init(symbol, config));

  // Discard unused pilots.
  prg->advance(starting_prb * NOF_DMRS_PER_RB * 2);

  // Generate sequence.
  prg->generate(sequence, M_SQRT1_2);
}

dmrs_pucch_processor_format2_impl::layer_dmrs_pattern
dmrs_pucch_processor_format2_impl::generate_dmrs_pattern(const config_t& config)
{
  layer_dmrs_pattern mask;

  // RE allocation pattern is always the same.
  mask.re_pattern = format2_prb_re_mask;

  // Set used PRB.
  mask.rb_mask.resize(config.starting_prb + config.nof_prb);
  mask.rb_mask.fill(config.starting_prb, config.starting_prb + config.nof_prb, true);

  if (config.intra_slot_hopping) {
    // Set second hop PRB allocation.
    mask.rb_mask2.resize(config.second_hop_prb + config.nof_prb);
    mask.rb_mask2.fill(config.second_hop_prb, config.second_hop_prb + config.nof_prb, true);

    // Set the hopping symbol index, indicating the start of the second hop. Recall that PUCCH Format 1 occupies a
    // maximum of 2 OFDM symbols.
    mask.hopping_symbol_index = config.start_symbol_index + 1;
  }

  // Set used symbols.
  mask.symbols.resize(config.start_symbol_index + config.nof_symbols);
  mask.symbols.fill(config.start_symbol_index, config.start_symbol_index + config.nof_symbols, true);

  return mask;
}

void dmrs_pucch_processor_format2_impl::estimate(channel_estimate&                     estimate,
                                                 const resource_grid_reader&           grid,
                                                 const dmrs_pucch_processor::config_t& config)
{
  srsran_assert((!config.intra_slot_hopping || (config.nof_symbols > pucch_constants::FORMAT2_MIN_NSYMB)),
                "Frequency hopping requires {} OFDM symbols.",
                pucch_constants::FORMAT2_MAX_NSYMB);

  unsigned nof_rx_ports = config.ports.size();

  // Prepare DM-RS symbol buffer dimensions.
  re_measurement_dimensions dims;
  dims.nof_subc    = config.nof_prb * NOF_DMRS_PER_RB;
  dims.nof_symbols = config.nof_symbols;
  dims.nof_slices  = pucch_constants::MAX_LAYERS;

  // Resize DM-RS symbol buffer.
  temp_symbols.resize(dims);

  unsigned i_symb_start = config.start_symbol_index;
  unsigned i_symb_end   = config.start_symbol_index + dims.nof_symbols;
  // For each symbol carrying DM-RS (up to 2 symbols maximum).
  for (unsigned i_symb = i_symb_start, i_dmrs_symb = 0; i_symb != i_symb_end; ++i_symb, ++i_dmrs_symb) {
    // Generate sequence.
    unsigned prb_start =
        ((i_symb != i_symb_start) && (config.intra_slot_hopping)) ? config.second_hop_prb : config.starting_prb;
    generate_sequence(temp_symbols.get_symbol(i_dmrs_symb, 0), i_symb, prb_start, config);
  }

  // Prepare channel estimator configuration.
  port_channel_estimator::configuration est_cfg;
  est_cfg.scs          = to_subcarrier_spacing(config.slot.numerology());
  est_cfg.cp           = config.cp;
  est_cfg.first_symbol = config.start_symbol_index;
  est_cfg.nof_symbols  = config.nof_symbols;
  est_cfg.dmrs_pattern = {generate_dmrs_pattern(config)};
  est_cfg.rx_ports     = config.ports;
  est_cfg.scaling      = 1.0F;

  // Perform estimation for each receive port.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    ch_estimator->compute(estimate, grid, i_port, temp_symbols, est_cfg);
  }
}