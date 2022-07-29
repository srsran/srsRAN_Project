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
  re_measurement_dimensions re_dims = {};
  re_dims.nof_prb                   = config.freq_allocation.get_nof_rb();
  re_dims.nof_symbols               = config.nof_symbols;
  re_dims.nof_slices                = config.nof_rx_ports;

  // Get REs from the resource grid.
  ch_symbols.resize(re_dims);
  get_ch_symbols(ch_symbols, grid, config);

  // Prepare internal buffers.
  re_dims.nof_slices = config.nof_tx_layers;
  mod_symbols_eq.resize(re_dims);
  noise_vars_eq.resize(re_dims);

  // Equalize channels and, for each Tx layer, combine contribution from all Rx antenna ports.
  equalizer->equalize(mod_symbols_eq, noise_vars_eq, ch_symbols, estimates);

  // Remove REs that were assigned to DM-RS symbols or reserved.
  remove_dmrs(mod_symbols_data, noise_vars_data, mod_symbols_eq, noise_vars_eq, config);

  // Build LLRs from channel symbols.
  demapper->demodulate_soft(data, mod_symbols_data, noise_vars_data, config.modulation);

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
