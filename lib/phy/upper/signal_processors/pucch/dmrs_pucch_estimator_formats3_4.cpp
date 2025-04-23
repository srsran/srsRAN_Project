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

#include "dmrs_pucch_estimator_formats3_4.h"
#include "srsran/phy/upper/pucch_formats3_4_helpers.h"
#include "srsran/ran/pucch/pucch_constants.h"

using namespace srsran;

void dmrs_pucch_estimator_formats3_4::generate_sequence(span<cf_t>             sequence,
                                                        const estimate_config& config,
                                                        unsigned               u,
                                                        unsigned               v,
                                                        unsigned               symbol)
{
  unsigned m_0 = 0;
  // m_0 for PUCCH Format 4 is selected according to TS38.211 Table 6.4.1.3.3.1-1.
  switch (config.occ_index) {
    case 0:
      m_0 = 0;
      break;
    case 1:
      m_0 = 6;
      break;
    case 2:
      m_0 = 3;
      break;
    case 3:
      m_0 = 9;
      break;
    default:
      break;
  }

  // Compute alpha index.
  unsigned alpha_idx = helper.get_alpha_index(config.slot, config.cp, config.n_id, symbol, m_0, 0);

  // Generate the r_uv sequence.
  low_papr_generator->generate(sequence, u, v, alpha_idx, NRE);
}

dmrs_pucch_estimator_formats3_4::layer_dmrs_pattern
dmrs_pucch_estimator_formats3_4::generate_dmrs_pattern(const estimate_config& config)
{
  layer_dmrs_pattern mask;

  // All RE within the PRB are allocated to DM-RS.
  mask.re_pattern = ~bounded_bitset<NRE>(NRE);

  // Set PRB allocation.
  mask.rb_mask.resize(config.starting_prb + config.nof_prb);
  mask.rb_mask.fill(config.starting_prb, config.starting_prb + config.nof_prb, true);

  if (config.second_hop_prb.has_value()) {
    // Set second hop PRB allocation.
    mask.rb_mask2.resize(*config.second_hop_prb + config.nof_prb);
    mask.rb_mask2.fill(*config.second_hop_prb, *config.second_hop_prb + config.nof_prb, true);

    // Set the hopping symbol index, indicating the start of the second hop. In case of a PUCCH allocation with an odd
    // number of symbols, the second hop is one symbol larger than the first one.
    mask.hopping_symbol_index = config.start_symbol_index + (config.nof_symbols / 2);
  }

  symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);

  mask.symbols.resize(get_nsymb_per_slot(config.cp));
  dmrs_symb_mask.for_each(0, config.nof_symbols, [&mask, &config](unsigned bitpos) {
    mask.symbols.set(config.start_symbol_index + bitpos);
  });

  return mask;
}

void dmrs_pucch_estimator_formats3_4::estimate(channel_estimate&           estimate,
                                               const resource_grid_reader& grid,
                                               const estimate_config&      config)
{
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);

  // Prepare DM-RS symbol buffer dimensions.
  re_measurement_dimensions dims;
  dims.nof_subc    = config.nof_prb * NRE;
  dims.nof_symbols = dmrs_symb_mask.count();
  dims.nof_slices  = pucch_constants::MAX_LAYERS;

  // Resize DM-RS symbol buffer.
  temp_symbols.resize(dims);

  // Compute group sequence.
  unsigned u;
  unsigned v;
  std::tie(u, v) = helper.compute_group_sequence(config.group_hopping, config.n_id);

  unsigned i_dmrs_symb = 0;
  for (unsigned i_symb = config.start_symbol_index, i_symb_end = config.start_symbol_index + config.nof_symbols;
       i_symb != i_symb_end;
       ++i_symb) {
    // For each symbol carrying DM-RS.
    if (!dmrs_symb_mask.test(i_symb - config.start_symbol_index)) {
      continue;
    }

    generate_sequence(temp_symbols.get_symbol(i_dmrs_symb, 0), config, u, v, i_symb);
    ++i_dmrs_symb;
  }

  // Prepare channel estimator configuration.
  port_channel_estimator::configuration est_cfg;
  est_cfg.scs          = to_subcarrier_spacing(config.slot.numerology());
  est_cfg.cp           = config.cp;
  est_cfg.first_symbol = config.start_symbol_index;
  est_cfg.nof_symbols  = config.nof_symbols;
  est_cfg.dmrs_pattern.assign(1, generate_dmrs_pattern(config));
  est_cfg.rx_ports = config.ports;
  est_cfg.scaling  = 1.0F;

  // Perform estimation for each receive port.
  for (unsigned i_port = 0, nof_rx_ports = config.ports.size(); i_port != nof_rx_ports; ++i_port) {
    ch_estimator->compute(estimate, grid, i_port, temp_symbols, est_cfg);
  }
}

void dmrs_pucch_estimator_formats3_4::estimate(channel_estimate&                                  ch_estimate,
                                               const resource_grid_reader&                        grid,
                                               const dmrs_pucch_estimator::format3_configuration& config)
{
  estimate_config est_cfg = {
      .slot               = config.slot,
      .cp                 = config.cp,
      .group_hopping      = config.group_hopping,
      .n_id               = config.n_id,
      .nof_prb            = config.nof_prb,
      .nof_symbols        = config.nof_symbols,
      .starting_prb       = config.starting_prb,
      .second_hop_prb     = config.second_hop_prb,
      .start_symbol_index = config.start_symbol_index,
      .additional_dmrs    = config.additional_dmrs,
      .occ_index          = 0,
      .ports              = config.ports,
  };

  estimate(ch_estimate, grid, est_cfg);
}

void dmrs_pucch_estimator_formats3_4::estimate(channel_estimate&                                  ch_estimate,
                                               const resource_grid_reader&                        grid,
                                               const dmrs_pucch_estimator::format4_configuration& config)
{
  estimate_config est_cfg = {
      .slot               = config.slot,
      .cp                 = config.cp,
      .group_hopping      = config.group_hopping,
      .n_id               = config.n_id,
      .nof_prb            = 1,
      .nof_symbols        = config.nof_symbols,
      .starting_prb       = config.starting_prb,
      .second_hop_prb     = config.second_hop_prb,
      .start_symbol_index = config.start_symbol_index,
      .additional_dmrs    = config.additional_dmrs,
      .occ_index          = config.occ_index,
      .ports              = config.ports,
  };

  estimate(ch_estimate, grid, est_cfg);
}
