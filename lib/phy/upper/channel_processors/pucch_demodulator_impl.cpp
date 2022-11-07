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
/// \brief PUCCH demodulator implementation definition.

#include "pucch_demodulator_impl.h"

using namespace srsgnb;

void pucch_demodulator_impl::demodulate(span<srsgnb::log_likelihood_ratio>                      llr,
                                        const srsgnb::resource_grid_reader&                     grid,
                                        const srsgnb::channel_estimate&                         estimates,
                                        const srsgnb::pucch_demodulator::format2_configuration& config)
{
  // Number of receive antenna ports.
  unsigned nof_rx_ports = static_cast<unsigned>(config.rx_ports.size());

  // Number of data Resource Elements in a slot for a single Rx port.
  unsigned nof_re_port = static_cast<unsigned>(PUCCH_FORMAT2_NOF_DATA_SC * config.nof_prb * config.nof_symbols);

  // Assert that allocations are valid.
  srsgnb_assert(config.nof_prb && config.nof_prb <= PUCCH_FORMAT2_MAX_NPRB,
                "Invalid Number of PRB allocated to PUCCH Format 2, i.e., {}. Valid range is 1 to {}.",
                config.nof_prb,
                PUCCH_FORMAT2_MAX_NPRB);

  srsgnb_assert(config.nof_symbols && config.nof_symbols <= PUCCH_FORMAT2_MAX_NSYMB,
                "Invalid Number of OFDM symbols allocated to PUCCH Format 2, i.e., {}. Valid range is 1 to {}.",
                config.nof_symbols,
                PUCCH_FORMAT2_MAX_NSYMB);

  // Resize data and channel estimation buffers.
  ch_re.resize({nof_re_port, nof_rx_ports});
  ch_estimates.resize({nof_re_port, nof_rx_ports, SINGLE_TX_LAYER});

  // Resize equalized data and post equalization noise variance buffers.
  eq_re.resize({nof_re_port, SINGLE_TX_LAYER});
  eq_noise_vars.resize({nof_re_port, SINGLE_TX_LAYER});

  // Extract data RE and channel estimation coefficients.
  get_data_re_ests(grid, estimates, config);

  // Extract the Rx port noise variances from the channel estimation.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    noise_var_estimates[i_port] = estimates.get_noise_variance(i_port, 0);
  }

  // Equalize the data RE.
  equalizer->equalize(
      eq_re, eq_noise_vars, ch_re, ch_estimates, span<float>(noise_var_estimates).first(nof_rx_ports), 1.0F);

  // Assert that the number of RE returned by the channel equalizer matches the expected number of LLR.
  srsgnb_assert(eq_re.get_dimension_size(channel_equalizer::re_list::dims::re) ==
                    llr.size() / get_bits_per_symbol(modulation_scheme::QPSK),
                "Number of equalized RE (i.e. {}) does not match the expected LLR data length (i.e. {})",
                eq_re.get_dimension_size(channel_equalizer::re_list::dims::re),
                llr.size() / get_bits_per_symbol(modulation_scheme::QPSK));

  // Apply soft symbol demodulation. PUCCH Format 2 modulation scheme is always QPSK, as per TS38.211 Section 6.3.2.5.2.
  demapper->demodulate_soft(llr, eq_re.get_view({0}), eq_noise_vars.get_view({0}), modulation_scheme::QPSK);

  // Descramble, as per TS38.211 Section 6.3.2.5.1.
  unsigned c_init = config.rnti * pow2(15) + config.n_id;
  descrambler->init(c_init);
  descrambler->apply_xor(llr, llr);
}

void pucch_demodulator_impl::demodulate(span<srsgnb::log_likelihood_ratio>                      llr,
                                        const srsgnb::resource_grid_reader&                     grid,
                                        const srsgnb::channel_estimate&                         estimates,
                                        const srsgnb::pucch_demodulator::format3_configuration& config)
{
  srsgnb_assertion_failure("PUCCH Format 3 not supported.");
}

void pucch_demodulator_impl::demodulate(span<srsgnb::log_likelihood_ratio>                      llr,
                                        const srsgnb::resource_grid_reader&                     grid,
                                        const srsgnb::channel_estimate&                         estimates,
                                        const srsgnb::pucch_demodulator::format4_configuration& config)
{
  srsgnb_assertion_failure("PUCCH Format 4 not supported.");
}

void pucch_demodulator_impl::get_data_re_ests(const resource_grid_reader&                     resource_grid,
                                              const channel_estimate&                         channel_ests,
                                              const pucch_demodulator::format2_configuration& config)
{
  // Prepare RB mask. RB allocation is contiguous for PUCCH Format 2.
  prb_mask.resize(config.nof_prb);
  prb_mask.fill(0, config.nof_prb, true);

  // Prepare RE mask.
  bounded_bitset<MAX_RB* NRE> re_mask = prb_mask.kronecker_product<NRE>(format2_prb_re_mask);

  // First OFDM subcarrier containing PUCCH Format 2.
  unsigned first_subc = config.first_prb * NRE;

  for (unsigned i_port = 0, i_port_end = config.rx_ports.size(); i_port != i_port_end; ++i_port) {
    // Get a view of the data RE destination buffer for a single Rx port.
    span<cf_t> re_port_buffer = ch_re.get_view({i_port});

    // Get a view of the channel estimates destination buffer for a single Rx port and Tx layer.
    span<cf_t> ests_port_buffer = ch_estimates.get_view({i_port, 0});

    for (unsigned i_symbol = config.start_symbol_index, i_symbol_end = config.start_symbol_index + config.nof_symbols;
         i_symbol != i_symbol_end;
         ++i_symbol) {
      // Extract data RE from the resource grid.
      re_port_buffer = resource_grid.get(re_port_buffer, i_port, i_symbol, first_subc, re_mask);

      // View over the channel estimation coefficients for a single OFDM symbol.
      span<const cf_t> ests_symbol = channel_ests.get_symbol_ch_estimate(i_symbol, i_port);

      // Copy channel estimation coefficients of the data RE into the destination buffer.
      re_mask.for_each(0, re_mask.size(), [&ests_port_buffer, &ests_symbol, &first_subc](unsigned bitpos) {
        ests_port_buffer.front() = ests_symbol[first_subc + bitpos];
        ests_port_buffer         = ests_port_buffer.last(ests_port_buffer.size() - 1);
      });
    }

    // Assert that all port data RE buffer elements have been filled.
    srsgnb_assert(re_port_buffer.empty(),
                  "Number of extracted port data RE does not match destination buffer dimensions: "
                  "{} unused elements.",
                  re_port_buffer.size());

    // Assert that all port channel estimates buffer elements have been filled.
    srsgnb_assert(
        ests_port_buffer.empty(),
        "Number of extracted port channel estimation coefficients does not match destination buffer dimensions: "
        "{} unused elements.",
        ests_port_buffer.size());
  }
}