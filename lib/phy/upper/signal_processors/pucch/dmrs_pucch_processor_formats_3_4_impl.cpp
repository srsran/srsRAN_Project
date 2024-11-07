/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_pucch_processor_formats_3_4_impl.h"
#include "srsran/phy/upper/pucch_formats_3_4_helpers.h"
#include "srsran/ran/pucch/pucch_constants.h"

using namespace srsran;

void dmrs_pucch_processor_formats_3_4_impl::generate_sequence(span<cf_t>                        sequence,
                                                              const config_t&                   config,
                                                              const sequence_generation_config& gen_config,
                                                              unsigned                          symbol)
{
  unsigned m_0 = 0;
  // m_0 for PUCCH Format 4 is selected according to TS38.211 Table 6.4.1.3.3.1-1.
  if (config.format == pucch_format::FORMAT_4) {
    switch (config.time_domain_occ) {
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
  }

  // Compute alpha index.
  unsigned alpha_idx = helper.get_alpha_index(config.slot, config.cp, config.n_id, symbol, m_0, 0);

  // Generate the r_uv sequence.
  low_papr_generator->generate(sequence, gen_config.u, gen_config.v, alpha_idx, NRE);
}

dmrs_pucch_processor_formats_3_4_impl::layer_dmrs_pattern
dmrs_pucch_processor_formats_3_4_impl::generate_dmrs_pattern(const config_t& config)
{
  layer_dmrs_pattern mask;

  // All RE within the PRB are allocated to DM-RS.
  mask.re_pattern = ~bounded_bitset<NRE>(NRE);

  // Set PRB allocation.
  mask.rb_mask.resize(config.starting_prb + config.nof_prb);
  mask.rb_mask.fill(config.starting_prb, config.starting_prb + config.nof_prb, true);

  if (config.intra_slot_hopping) {
    // Set second hop PRB allocation.
    mask.rb_mask2.resize(config.second_hop_prb + config.nof_prb);
    mask.rb_mask2.fill(config.second_hop_prb, config.second_hop_prb + config.nof_prb, true);

    // Set the hopping symbol index, indicating the start of the second hop. In case of a PUCCH allocation with an odd
    // number of symbols, the second hop is one symbol larger than the first one.
    mask.hopping_symbol_index = config.start_symbol_index + (config.nof_symbols / 2);
  }

  symbol_slot_mask dmrs_symb_mask =
      get_pucch_formats_3_4_dmrs_symbol_mask(config.nof_symbols, config.intra_slot_hopping, config.additional_dmrs);

  mask.symbols.resize(get_nsymb_per_slot(config.cp));
  dmrs_symb_mask.for_each(0, config.nof_symbols, [&mask, &config](unsigned bitpos) {
    mask.symbols.set(config.start_symbol_index + bitpos);
  });

  return mask;
}

void dmrs_pucch_processor_formats_3_4_impl::estimate(channel_estimate&           estimate,
                                                     const resource_grid_reader& grid,
                                                     const config_t&             config)
{
  srsran_assert(!((config.format == pucch_format::FORMAT_4) && (config.nof_prb > 1)),
                "PUCCH Format 4 occupies a single PRB");

  unsigned nof_rx_ports = config.ports.size();

  symbol_slot_mask dmrs_symb_mask =
      get_pucch_formats_3_4_dmrs_symbol_mask(config.nof_symbols, config.intra_slot_hopping, config.additional_dmrs);

  // Prepare DM-RS symbol buffer dimensions.
  re_measurement_dimensions dims;
  dims.nof_subc    = config.nof_prb * NRE;
  dims.nof_symbols = dmrs_symb_mask.count();
  dims.nof_slices  = pucch_constants::MAX_LAYERS;

  // Resize DM-RS symbol buffer.
  temp_symbols.resize(dims);

  unsigned u, v;
  helper.compute_group_sequence(config.group_hopping, config.n_id, u, v);

  unsigned i_symb_start = config.start_symbol_index;
  unsigned i_symb_end   = config.start_symbol_index + config.nof_symbols;

  unsigned i_dmrs_symb = 0;
  for (unsigned i_symb = i_symb_start; i_symb != i_symb_end; ++i_symb) {
    // For each symbol carrying DM-RS.
    if (!dmrs_symb_mask.test(i_symb - config.start_symbol_index)) {
      continue;
    }

    sequence_generation_config cfg = {.u = u, .v = v};
    generate_sequence(temp_symbols.get_symbol(i_dmrs_symb, 0), config, cfg, i_symb);
    ++i_dmrs_symb;
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