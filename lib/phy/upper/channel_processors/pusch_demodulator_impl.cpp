/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// \file
/// \brief PUSCH demodulator implementation definition.

#include "pusch_demodulator_impl.h"
#include "srsran/ran/sch_dmrs_power.h"
#include "srsran/srsvec/mean.h"

using namespace srsran;

pusch_demodulator::demodulation_status pusch_demodulator_impl::demodulate(span<log_likelihood_ratio>  data,
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

  // Estimate Signal-to-Interference-plus-Noise Ratio.
  float mean_noise_var = srsvec::mean(eq_noise_vars.get_data());
  if (mean_noise_var > 0.0) {
    status.sinr_dB.emplace(-convert_power_to_dB(mean_noise_var));
  }

  // Assert that the number of RE returned by the channel equalizer matches the expected number of LLR.
  srsran_assert(nof_re_port * config.nof_tx_layers == data.size() / get_bits_per_symbol(config.modulation),
                "Number of equalized RE (i.e. {}) does not match the expected LLR data length (i.e. {})",
                nof_re_port * config.nof_tx_layers,
                data.size() / get_bits_per_symbol(config.modulation));

  // For now, layer demapping is not implemented.
  srsran_assert(config.nof_tx_layers == 1, "Only a single transmit layer is supported.");

  // Get the equalized resource elements and noise variances for a single transmit layer.
  span<const cf_t>  eq_re_flat   = eq_re.get_view({0});
  span<const float> eq_vars_flat = eq_noise_vars.get_view({0});

  // Build LLRs from channel symbols.
  demapper->demodulate_soft(data, eq_re_flat, eq_vars_flat, config.modulation);

  // Calculate EVM only if it is available.
  if (evm_calc) {
    status.evm.emplace(evm_calc->calculate(data, eq_re_flat, config.modulation));
  }

  // Descramble.
  descramble(data, data, config);

  return status;
}

void pusch_demodulator_impl::descramble(span<srsran::log_likelihood_ratio>              out,
                                        span<const srsran::log_likelihood_ratio>        in,
                                        const srsran::pusch_demodulator::configuration& config)
{
  // Initialise sequence.
  unsigned c_init = config.rnti * pow2(15) + config.n_id;
  descrambler->init(c_init);

  // Keeps the index of the previous placeholder.
  unsigned last_placeholder = 0;

  // For each placeholder...
  config.placeholders.for_each(
      config.modulation, config.nof_tx_layers, [&](unsigned y_placeholder, unsigned nof_x_placeholders) {
        // The distance between two repetition placeholders must be larger than 1.
        srsran_assert(y_placeholder > last_placeholder,
                      "Placeholder must be in ascending order with steps greater than 1.");

        // Calculate the number of elements to process, exclude the element before the placeholder.
        unsigned nof_elements = y_placeholder - last_placeholder - 1;

        // Update last placeholder.
        last_placeholder = y_placeholder + 1 + nof_x_placeholders;

        // Applies XOR until the element before the placeholder.
        descrambler->apply_xor(out.first(nof_elements), in.first(nof_elements));
        out = out.last(out.size() - nof_elements);
        in  = in.last(in.size() - nof_elements);

        // Extracts the mask of the next element.
        std::array<uint8_t, 1> temp_in  = {};
        std::array<uint8_t, 1> temp_out = {};
        descrambler->apply_xor(temp_out, temp_in);
        bool toggle = (temp_in.front() != temp_out.front());

        // Discards scrambling chip.
        descrambler->advance(1 + nof_x_placeholders);

        // Process repetition placeholder.
        for (unsigned i_llr = 0; i_llr != 2; ++i_llr) {
          out.front() = toggle ? -in.front() : in.front();
          out         = out.last(out.size() - 1);
          in          = in.last(in.size() - 1);
        }

        // Process x placeholders.
        srsvec::copy(out.first(nof_x_placeholders), in.first(nof_x_placeholders));
        out = out.last(out.size() - nof_x_placeholders);
        in  = in.last(in.size() - nof_x_placeholders);
      });

  // Process remaining data.
  descrambler->apply_xor(out, in);
}
