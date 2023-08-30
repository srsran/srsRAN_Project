/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUSCH demodulator implementation definition.

#include "pusch_demodulator_impl.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "srsran/ran/sch_dmrs_power.h"
#include "srsran/srsvec/mean.h"

using namespace srsran;

pusch_demodulator::demodulation_status pusch_demodulator_impl::demodulate(pusch_codeword_buffer&      codeword_buffer,
                                                                          const resource_grid_reader& grid,
                                                                          const channel_estimate&     estimates,
                                                                          const configuration&        config)
{
  demodulation_status status = {};

  // Number of OFDM symbols containing DM-RS in a slot.
  unsigned nof_dmrs_symbols = std::accumulate(config.dmrs_symb_pos.begin(), config.dmrs_symb_pos.end(), 0);

  // Number of receive antenna ports.
  unsigned nof_rx_ports = static_cast<unsigned>(config.rx_ports.size());

  // Calculate the actual number of RE for data in each PRB.
  unsigned nof_data_re_prb = NRE * config.nof_symbols - config.dmrs_config_type.nof_dmrs_per_rb() *
                                                            config.nof_cdm_groups_without_data * nof_dmrs_symbols;

  // Number of data Resource Elements in a slot for a single Rx port.
  unsigned nof_re_port = static_cast<unsigned>(config.rb_mask.count() * nof_data_re_prb);

  // Initialise sequence.
  unsigned c_init = config.rnti * pow2(15) + config.n_id;
  descrambler->init(c_init);

  // Resize equalizer input buffers.
  ch_re.resize({nof_re_port, nof_rx_ports});
  ch_estimates.resize({nof_re_port, nof_rx_ports, config.nof_tx_layers});

  // Resize equalizer output buffers.
  eq_re.resize({nof_re_port, config.nof_tx_layers});
  eq_noise_vars.resize({nof_re_port, config.nof_tx_layers});

  // Extract the data symbols and channel estimates from the resource grid.
  get_ch_data_re(ch_re, grid, config);
  get_ch_data_estimates(ch_estimates, estimates, config);

  // Extract the Rx port noise variances from the channel estimation.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    noise_var_estimates[i_port] = estimates.get_noise_variance(i_port, 0);
  }

  // Equalize channels and, for each Tx layer, combine contribution from all Rx antenna ports.
  equalizer->equalize(
      eq_re, eq_noise_vars, ch_re, ch_estimates, span<float>(noise_var_estimates).first(nof_rx_ports), 1.0F);

  // Estimate post equalization Signal-to-Interference-plus-Noise Ratio.
  if (compute_post_eq_sinr) {
    float mean_noise_var = srsvec::mean(eq_noise_vars.get_data());
    if (mean_noise_var > 0.0) {
      status.sinr_dB.emplace(-convert_power_to_dB(mean_noise_var));
    }
  }

  // For now, layer demapping is not implemented.
  srsran_assert(config.nof_tx_layers == 1, "Only a single transmit layer is supported.");

  // Get the equalized resource elements and noise variances for a single transmit layer.
  span<const cf_t>  eq_re_flat   = eq_re.get_view({0});
  span<const float> eq_vars_flat = eq_noise_vars.get_view({0});

  // Get codeword buffer. Use codeword_buffer.get_next_block_view(codeword_size) in next revision.
  unsigned codeword_size              = nof_re_port * config.nof_tx_layers * get_bits_per_symbol(config.modulation);
  span<log_likelihood_ratio> codeword = span<log_likelihood_ratio>(temp_codeword).first(codeword_size);

  // Build LLRs from channel symbols.
  demapper->demodulate_soft(codeword, eq_re_flat, eq_vars_flat, config.modulation);

  // Calculate EVM only if it is available.
  if (evm_calc) {
    status.evm.emplace(evm_calc->calculate(codeword, eq_re_flat, config.modulation));
  }

  // Process remaining data.
  span<log_likelihood_ratio> codeword_descr = span<log_likelihood_ratio>(temp_codeword_descr).first(codeword_size);
  descrambler->apply_xor(codeword_descr, codeword);

  // Currently the codeword is processed in a single block and after scrambling.
  codeword_buffer.on_new_block(codeword, codeword_descr);
  codeword_buffer.on_end_codeword();

  return status;
}
