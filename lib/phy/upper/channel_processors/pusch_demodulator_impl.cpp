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
#include "srsgnb/ran/sch_dmrs_power.h"

using namespace srsgnb;

void pusch_demodulator_impl::demodulate(span<log_likelihood_ratio>  data,
                                        span<log_likelihood_ratio>  harq_ack,
                                        span<log_likelihood_ratio>  csi_part1,
                                        span<log_likelihood_ratio>  csi_part2,
                                        const resource_grid_reader& grid,
                                        const channel_estimate&     estimates,
                                        const configuration&        config)
{
  std::array<unsigned, dims::nof_dims> re_dims = {static_cast<unsigned>(config.rb_mask.count() * NRE),
                                                  config.nof_symbols,
                                                  static_cast<unsigned>(config.rx_ports.size())};

  // Get REs from the resource grid.
  ch_symbols.resize(re_dims);
  get_ch_symbols(ch_symbols, grid, config);

  // Prepare internal buffers (the number of PRBs and OFDM symbols is the same as above).
  re_dims[dims::slice] = config.nof_tx_layers;
  mod_symbols_eq.resize(re_dims);
  noise_vars_eq.resize(re_dims);

  // Get PUSCH data to DM-RS scaling ratio in linear.
  float scaling = convert_dB_to_amplitude(get_sch_to_dmrs_ratio_dB(config.nof_cdm_groups_without_data));

  // Equalize channels and, for each Tx layer, combine contribution from all Rx antenna ports.
  equalizer->equalize(mod_symbols_eq, noise_vars_eq, ch_symbols, estimates, scaling);

  unsigned    nof_symbols      = data.size() / get_bits_per_symbol(config.modulation);
  span<cf_t>  mod_symbols_data = span<cf_t>(temp_mod_symbols_data).first(nof_symbols);
  span<float> noise_vars_data  = span<float>(temp_noise_vars_data).first(nof_symbols);

  // Remove REs that were assigned to DM-RS symbols or reserved.
  remove_dmrs(mod_symbols_data, noise_vars_data, mod_symbols_eq, noise_vars_eq, config);

  // Build LLRs from channel symbols.
  demapper->demodulate_soft(data, mod_symbols_data, noise_vars_data, config.modulation);

  // Descramble.
  unsigned c_init = config.rnti * pow2(15) + config.n_id;
  descrambler->init(c_init);
  // Temporarily, UCI placeholders for 1-bit HARQ-ACK transmissions are not considered.
  descrambler->apply_xor(data, data);

  // Extract HARQ ACK soft bits.
  extract_harq_ack(harq_ack, data, config);

  // Extract CSI Part1 soft bits.
  extract_csi_part1(csi_part1, data, config);

  // Extract CSI Part2 soft bits.
  extract_csi_part2(csi_part2, data, config);
}
