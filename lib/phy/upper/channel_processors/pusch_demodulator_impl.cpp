/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUSCH demodulator implementation definition.

#include "pusch_demodulator_impl.h"

using namespace srsgnb;

void pusch_demodulator_impl::demodulate(data_llr_buffer&            data,
                                        harq_ack_llr_buffer&        harq_ack,
                                        csi_part1_llr&              csi_part1,
                                        csi_part2_llr&              csi_part2,
                                        const resource_grid_reader& grid,
                                        const channel_estimate&     estimates,
                                        const configuration&        config)
{
  // Configure equalizer.
  channel_equalizer::configuration eq_cfg;
  eq_cfg.nof_rx_ports  = config.nof_rx_ports;
  eq_cfg.nof_tx_layers = config.nof_tx_layers;
  eq_cfg.rb_mask       = config.freq_allocation.get_prb_mask(config.bwp_start_rb, config.bwp_size_rb);
  eq_cfg.first_symbol  = config.start_symbol_index;
  eq_cfg.nof_symbols   = config.nof_symbols;

  // Resize internal buffers.
  unsigned bits_per_symbol = get_bits_per_symbol(config.modulation);
  mod_symbols.resize(data.size() / bits_per_symbol);
  noise_vars.resize(mod_symbols.size());

  // Equalize channels and, for each Tx layer, combine contribution from all Rx antenna ports.
  equalizer->equalize(mod_symbols, noise_vars, grid, estimates, eq_cfg);

  // Build LLRs from channel symbols.
  demapper->demodulate_soft(data, mod_symbols, noise_vars, config.modulation);

  // Descramble.
  unsigned c_init = config.rnti * pow2(15) + config.n_id;
  descrambler->init(c_init);
  // To descramble, LLRs should be seen as int8_t. Note that descrambling only changes the sign of some elements and,
  // thus, this operation is safe.
  span<int8_t> data_tmp((int8_t*)data.begin(), data.size());
  // The following operation changes data_tmp and, in turn, data.
  // todo(david): This is temporary, the true descrambler should take into account the UCI placeholders.
  descrambler->apply_xor(data_tmp, data_tmp);

  // Extract HARQ ACK soft bits.
  extract_harq_ack(harq_ack, data, config);

  // Extract CSI Part1 soft bits.
  extract_csi_part1(csi_part1, data, config);

  // Extract CSI Part2 soft bits.
  extract_csi_part2(csi_part2, data, config);
}
