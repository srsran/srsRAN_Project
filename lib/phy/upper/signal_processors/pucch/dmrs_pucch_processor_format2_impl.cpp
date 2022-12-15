/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_pucch_processor_format2_impl.h"
#include "srsgnb/srsvec/prod.h"

using namespace srsgnb;

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

void dmrs_pucch_processor_format2_impl::sequence_generation(span<cf_t>                            sequence,
                                                            unsigned                              symbol,
                                                            unsigned                              starting_prb,
                                                            const dmrs_pucch_processor::config_t& config)
{
  // Initialise pseudo-random generator.
  prg->init(c_init(symbol, config));

  // Discard unused pilots.
  prg->advance(starting_prb * NOF_DMRS_PER_RB * 2);

  // Generate sequence.
  prg->generate(sequence.first(config.nof_prb * NOF_DMRS_PER_RB), M_SQRT1_2);
}

void dmrs_pucch_processor_format2_impl::generate_dmrs_pattern(layer_dmrs_pattern& mask, const config_t& config)
{
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
}

void dmrs_pucch_processor_format2_impl::estimate(channel_estimate&                     estimate,
                                                 const resource_grid_reader&           grid,
                                                 const dmrs_pucch_processor::config_t& config)
{
  srsgnb_assert((!config.intra_slot_hopping || (config.nof_symbols > PUCCH_FORMAT2_MIN_NSYMB)),
                "Frequency hopping requires {} OFDM symbols.",
                PUCCH_FORMAT2_MAX_NSYMB);

  unsigned nof_rx_ports = config.ports.size();

  // Prepare DM-RS symbol buffer dimensions.
  re_measurement_dimensions dims;
  dims.nof_subc    = config.nof_prb * NOF_DMRS_PER_RB;
  dims.nof_symbols = config.nof_symbols;
  dims.nof_slices  = PUCCH_MAX_LAYERS;

  // Resize DM-RS symbol buffer.
  temp_symbols.resize(dims);

  // Find the highest starting PRB.
  unsigned max_prb_start =
      config.intra_slot_hopping ? std::max(config.starting_prb, config.second_hop_prb) : config.starting_prb;

  // Resize channel estimate.
  estimate.resize({max_prb_start + config.nof_prb,
                   config.start_symbol_index + config.nof_symbols,
                   nof_rx_ports,
                   PUCCH_MAX_LAYERS});

  // Generate the DM-RS allocation pattern.
  generate_dmrs_pattern(temp_pattern, config);

  unsigned i_symb_start = config.start_symbol_index;
  unsigned i_symb_end   = config.start_symbol_index + config.nof_symbols;
  // For each symbol carrying DM-RS (up to 2 symbols maximum).
  for (unsigned i_symb = i_symb_start, i_dmrs_symb = 0; i_symb != i_symb_end; ++i_symb, ++i_dmrs_symb) {
    // Generate sequence.
    unsigned prb_start = ((i_symb != 0) && (config.intra_slot_hopping)) ? config.second_hop_prb : config.starting_prb;
    sequence_generation(temp_symbols.get_symbol(i_dmrs_symb, 0), i_symb, prb_start, config);
  }

  // Prepare channel estimator configuration.
  port_channel_estimator::configuration est_cfg = {};
  est_cfg.dmrs_pattern.emplace_back(temp_pattern);
  est_cfg.scs          = to_subcarrier_spacing(config.slot.numerology());
  est_cfg.first_symbol = config.start_symbol_index;
  est_cfg.nof_symbols  = config.nof_symbols;
  est_cfg.rx_ports     = config.ports;

  // Perform estimation for each receive port.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    ch_estimator->compute(estimate, grid, i_port, temp_symbols, est_cfg);
  }
}