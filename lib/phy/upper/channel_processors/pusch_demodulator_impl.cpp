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
                                        const resource_grid_reader& grid,
                                        const channel_estimate&     estimates,
                                        const configuration&        config)
{
  // Number of OFDM symbols containing DM-RS in a slot.
  unsigned nof_dmrs_symbols = std::accumulate(config.dmrs_symb_pos.begin(), config.dmrs_symb_pos.end(), 0);

  // Number of receive antenna ports.
  unsigned nof_rx_ports = static_cast<unsigned>(config.rx_ports.size());

  // Number of data Resource Elements in a slot for a single Rx port.
  unsigned nof_re_port = static_cast<unsigned>(config.rb_mask.count() * NRE) * (config.nof_symbols - nof_dmrs_symbols);

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
  descramble(data, data, config);
}

void pusch_demodulator_impl::descramble(span<srsgnb::log_likelihood_ratio>              out,
                                        span<const srsgnb::log_likelihood_ratio>        in,
                                        const srsgnb::pusch_demodulator::configuration& config)
{
  // Initialise sequence.
  unsigned c_init = config.rnti * pow2(15) + config.n_id;
  descrambler->init(c_init);

  // Keeps the index of the previous placeholder.
  unsigned last_placeholder = 0;

  // For each placeholder...
  config.placeholders.for_each(config.modulation, config.nof_tx_layers, [&](unsigned placeholder) {
    // The distance between two repetition placeholders must be larger than 1.
    srsgnb_assert(placeholder > last_placeholder + 1,
                  "Placeholder must be in ascending order with steps greater than 1.");

    // Calculate the number of elements to process, exclude the element before the placeholder.
    unsigned nof_elements = placeholder - last_placeholder - 1;

    // Update last placeholder.
    last_placeholder = placeholder + 1;

    // Applies XOR until the element before the placeholder.
    descrambler->apply_xor(out.first(nof_elements), in.first(nof_elements));
    out = out.last(out.size() - nof_elements);
    in  = in.last(in.size() - nof_elements);

    // Extracts the mask of the next element.
    std::array<uint8_t, 1> temp_in  = {};
    std::array<uint8_t, 1> temp_out = {};
    descrambler->apply_xor_bit(temp_out, temp_in);
    bool toggle = (temp_in.front() != temp_out.front());

    // Discards scrambling chip.
    descrambler->advance(1);

    // Process repetition placeholder.
    for (unsigned i_llr = 0; i_llr != 2; ++i_llr) {
      out.front() = toggle ? -in.front() : in.front();
      out         = out.last(out.size() - 1);
      in          = in.last(in.size() - 1);
    }
  });

  // Process remaining data.
  descrambler->apply_xor(out, in);
}