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
  // Number of OFDM symbols containing DM-RS in a slot.
  unsigned nof_dmrs_symbols = std::accumulate(config.dmrs_symb_pos.begin(), config.dmrs_symb_pos.end(), 0);

  unsigned nof_rx_ports = static_cast<unsigned>(config.rx_ports.size());

  // Number of data Resource Elements in a slot for a single Rx port.
  unsigned nof_re_port = static_cast<unsigned>(config.rb_mask.count() * NRE) * (config.nof_symbols - nof_dmrs_symbols);

  // Get REs from the resource grid.
  ch_re.resize({nof_re_port, nof_rx_ports});
  ch_estimates.resize({nof_re_port, nof_rx_ports, config.nof_tx_layers});

  // Extract the data symbols and channel estimates from the resource grid.
  get_ch_data_re(ch_re, grid, config);
  get_ch_data_estimates(ch_estimates, estimates, config);

  // Extract the Rx port noise variances from the channel estimation.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    noise_var_estimates[i_port] = estimates.get_noise_variance(i_port, 0);
  }

  // Prepare internal buffers.
  eq_re.resize({nof_re_port, config.nof_tx_layers});
  eq_noise_vars.resize({nof_re_port, config.nof_tx_layers});

  // Get PUSCH data to DM-RS scaling ratio in linear.
  float scaling = convert_dB_to_amplitude(get_sch_to_dmrs_ratio_dB(config.nof_cdm_groups_without_data));

  // Equalize channels and, for each Tx layer, combine contribution from all Rx antenna ports.
  equalizer->equalize(
      eq_re, eq_noise_vars, ch_re, ch_estimates, span<float>(noise_var_estimates).first(nof_rx_ports), scaling);

  // Assert that the number of RE returned by the channel equalizer matches the expected number of LLR.
  srsgnb_assert(nof_re_port * config.nof_tx_layers == data.size() / get_bits_per_symbol(config.modulation),
                "Number of equalized RE (i.e. {}) does not match the expected LLR data length (i.e. {})",
                nof_re_port * config.nof_tx_layers,
                data.size() / get_bits_per_symbol(config.modulation));

  // For now, layer demapping is not implemented.
  srsgnb_assert(config.nof_tx_layers == 1, "Only a single transmit layer is supported.");

  // Get the equalized resource elements and noise variances for a single transmit layer.
  span<const cf_t>  eq_re_flat   = eq_re.get_view({0});
  span<const float> eq_vars_flat = eq_noise_vars.get_view({0});

  // Build LLRs from channel symbols.
  demapper->demodulate_soft(data, eq_re_flat, eq_vars_flat, config.modulation);

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
