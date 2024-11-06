/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
/// \brief PUCCH Format 3 demodulator definition.

#include "pucch_demodulator_format3.h"
#include "srsran/phy/support/mask_types.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/equalization/modular_ch_est_list.h"
#include "srsran/phy/upper/pucch_formats_3_4_helpers.h"

using namespace srsran;

void pucch_demodulator_format3::demodulate(span<log_likelihood_ratio>                      llr,
                                           const resource_grid_reader&                     grid,
                                           const channel_estimate&                         estimates,
                                           const pucch_demodulator::format3_configuration& config)
{
  // Number of receive antenna ports.
  auto nof_rx_ports = static_cast<unsigned>(config.rx_ports.size());

  // Number of REs per OFDM symbol.
  const unsigned nof_re_symb = config.nof_prb * NRE;

  // Index of the first symbol allocated to the second subcarrier when intra-slot frequency hopping is enabled.
  const unsigned second_hop_start = (config.nof_symbols / 2) + config.start_symbol_index;

  // PUCCH Format 3 modulation scheme can be QPSK or pi/2-BPSK, as per TS38.211 Section 6.3.2.6.2.
  modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;

  // Number of data Resource Elements in a slot for a single Rx port.
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats_3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);

  const unsigned nof_re_port = (config.nof_symbols - dmrs_symb_mask.count()) * config.nof_prb * NRE;

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

  // Resize equalized data and post equalization noise variance buffers.
  eq_re.resize(nof_re_port);
  eq_noise_vars.resize(nof_re_port);

  // Extract the Rx port noise variances from the channel estimation.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    noise_var_estimates[i_port] = estimates.get_noise_variance(i_port, 0);
  }

  // Get a view on the equalized RE buffer.
  span<cf_t>  eq_re_view      = eq_re;
  span<float> noise_vars_view = eq_noise_vars;

  // Assert that the number of RE returned by the channel equalizer matches the expected number of LLR.
  srsran_assert(eq_re.size() == llr.size() / get_bits_per_symbol(mod_scheme),
                "Number of equalized RE (i.e. {}) does not match the expected LLR data length (i.e. {})",
                eq_re.size(),
                llr.size() / get_bits_per_symbol(mod_scheme));

  for (unsigned i_symbol = config.start_symbol_index, i_symbol_end = config.start_symbol_index + config.nof_symbols;
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // Skip DM-RS symbols.
    if (dmrs_symb_mask.test(i_symbol - config.start_symbol_index)) {
      continue;
    }

    // Calculate the lowest resource element containing PUCCH Format 3 within the OFDM symbol.
    unsigned first_subc = config.first_prb * NRE;
    if ((i_symbol >= second_hop_start) && config.second_hop_prb.has_value()) {
      // Intra-slot frequency hopping.
      first_subc = config.second_hop_prb.value() * NRE;
    }

    // Create modular buffers to hold the spans for this symbol.
    modular_re_buffer_reader<cbf16_t, MAX_PORTS> re_symb(config.rx_ports.size(), nof_re_symb);
    modular_ch_est_list<MAX_PORTS>               estimates_symb(nof_re_symb, config.rx_ports.size(), SINGLE_TX_LAYER);

    for (unsigned i_port = 0, i_port_end = nof_rx_ports; i_port != i_port_end; ++i_port) {
      // Extract data RE from the resource grid.
      unsigned            i_port_grid  = config.rx_ports[i_port];
      span<const cbf16_t> re_symb_view = grid.get_view(i_port_grid, i_symbol).subspan(first_subc, nof_re_symb);
      re_symb.set_slice(i_port, re_symb_view);

      // Extract estimates from the estimates buffer.
      estimates_symb.set_channel(
          estimates.get_symbol_ch_estimate(i_symbol, i_port).subspan(first_subc, nof_re_symb), i_port, 0);
    }

    // Get a view of the equalized RE buffer for a single symbol.
    span<cf_t> eq_re_symb = eq_re_view.first(nof_re_symb);

    // Get a view of the equalized RE buffer for a single symbol.
    span<float> eq_noise_vars_symb = noise_vars_view.first(nof_re_symb);

    // Equalize the data RE for a single symbol.
    equalizer->equalize(eq_re_symb,
                        eq_noise_vars_symb,
                        re_symb,
                        estimates_symb,
                        span<float>(noise_var_estimates).first(nof_rx_ports),
                        1.0F);

    // Revert transform precoding for a single symbol.
    precoder->deprecode_ofdm_symbol(eq_re_symb, eq_re_symb);

    // Advance the equalized RE and noise vars views.
    eq_re_view      = eq_re_view.subspan(nof_re_symb, eq_re_view.size() - nof_re_symb);
    noise_vars_view = noise_vars_view.subspan(nof_re_symb, noise_vars_view.size() - nof_re_symb);
  }

  // Apply soft symbol demodulation.
  demapper->demodulate_soft(llr, eq_re, eq_noise_vars, mod_scheme);

  // Descramble, as per TS38.211 Section 6.3.2.6.1.
  const unsigned c_init = (config.rnti * pow2(15)) + config.n_id;
  descrambler->init(c_init);
  descrambler->apply_xor(llr, llr);
}