/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUCCH Format 3 demodulator definition.

#include "pucch_demodulator_format3.h"
#include "pucch_formats_3_4_helpers.h"
#include "srsran/phy/support/mask_types.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/pucch_formats_3_4_helpers.h"

using namespace srsran;

void pucch_demodulator_format3::demodulate(span<log_likelihood_ratio>                      llr,
                                           const resource_grid_reader&                     grid,
                                           const channel_estimate&                         estimates,
                                           const pucch_demodulator::format3_configuration& config)
{
  // Number of receive antenna ports.
  auto nof_rx_ports = static_cast<unsigned>(config.rx_ports.size());

  // Number of data Resource Elements in a slot for a single Rx port.
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats_3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);
  unsigned nof_re_port = (config.nof_symbols - dmrs_symb_mask.count()) * config.nof_prb * NRE;

  // Assert that allocations are valid.
  srsran_assert(config.nof_prb && config.nof_prb <= pucch_constants::FORMAT3_MAX_NPRB,
                "Invalid Number of PRB allocated to PUCCH Format 3, i.e., {}. Valid range is 1 to {}.",
                config.nof_prb,
                pucch_constants::FORMAT3_MAX_NPRB);

  srsran_assert((config.first_prb + config.nof_prb) * NRE <= grid.get_nof_subc(),
                "PUCCH Format 3: PRB allocation outside grid (first hop). Requested [{}, {}), grid has {} PRBs.",
                config.first_prb,
                config.first_prb + config.nof_prb,
                grid.get_nof_subc() / NRE);

  srsran_assert(!config.second_hop_prb.has_value() ||
                    ((config.second_hop_prb.value() + config.nof_prb) * NRE <= grid.get_nof_subc()),
                "PUCCH Format 3: PRB allocation outside grid (second hop). Requested [{}, {}), grid has {} PRBs.",
                config.second_hop_prb.value(),
                config.second_hop_prb.value() + config.nof_prb,
                grid.get_nof_subc() / NRE);

  interval<unsigned, true> nof_symbols_range(pucch_constants::FORMAT3_MIN_NSYMB, pucch_constants::FORMAT3_MAX_NSYMB);
  srsran_assert(nof_symbols_range.contains(config.nof_symbols),
                "Invalid Number of OFDM symbols allocated to PUCCH Format 3, i.e., {}. Valid range is {}.",
                config.nof_symbols,
                nof_symbols_range);

  // Resize data and channel estimation buffers.
  ch_re.resize(nof_rx_ports, nof_re_port);
  ch_estimates.resize(nof_re_port, nof_rx_ports, SINGLE_TX_LAYER);

  // Resize equalized data and post equalization noise variance buffers.
  eq_re.resize(nof_re_port);
  eq_noise_vars.resize(nof_re_port);

  // Extract data RE and channel estimation coefficients.
  pucch_3_4_get_data_re_ests(ch_re,
                             ch_estimates,
                             grid,
                             estimates,
                             dmrs_symb_mask,
                             config.rx_ports.size(),
                             config.nof_symbols,
                             config.first_prb,
                             config.nof_prb,
                             config.second_hop_prb,
                             config.start_symbol_index);

  // Extract the Rx port noise variances from the channel estimation.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    noise_var_estimates[i_port] = estimates.get_noise_variance(i_port, 0);
  }

  // Equalize the data RE.
  equalizer->equalize(
      eq_re, eq_noise_vars, ch_re, ch_estimates, span<float>(noise_var_estimates).first(nof_rx_ports), 1.0F);

  // Number of REs per OFDM symbol
  unsigned nof_re_symb = config.nof_prb * NRE;
  // Get a view on the equalized RE buffer.
  span<cf_t> eq_re_view = eq_re;
  while (!eq_re_view.empty()) {
    // Get a view of the equalized RE buffer for a single symbol.
    span<cf_t> eq_re_symb = eq_re_view.subspan(0, nof_re_symb);
    // Revert transform precoding for a single symbol.
    precoder->deprecode_ofdm_symbol(eq_re_symb, eq_re_symb);
    // Advance the equalized RE view.
    eq_re_view = eq_re_view.subspan(nof_re_symb, eq_re_view.size() - nof_re_symb);
  }

  // PUCCH Format 3 modulation scheme can be QPSK or pi/2-BPSK, as per TS38.211 Section 6.3.2.6.2.
  modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;

  // Assert that the number of RE returned by the channel equalizer matches the expected number of LLR.
  srsran_assert(eq_re.size() == llr.size() / get_bits_per_symbol(mod_scheme),
                "Number of equalized RE (i.e. {}) does not match the expected LLR data length (i.e. {})",
                eq_re.size(),
                llr.size() / get_bits_per_symbol(mod_scheme));

  // Apply soft symbol demodulation.
  demapper->demodulate_soft(llr, eq_re, eq_noise_vars, mod_scheme);

  // Descramble, as per TS38.211 Section 6.3.2.6.1.
  unsigned c_init = config.rnti * pow2(15) + config.n_id;
  descrambler->init(c_init);
  descrambler->apply_xor(llr, llr);
}