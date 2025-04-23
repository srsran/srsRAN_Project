/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/phy/upper/pucch_formats3_4_helpers.h"

using namespace srsran;

void pucch_demodulator_format3::demodulate(span<log_likelihood_ratio>                      llr,
                                           const resource_grid_reader&                     grid,
                                           const channel_estimate&                         estimates,
                                           const pucch_demodulator::format3_configuration& config)
{
  // PUCCH Format 3 modulation scheme can be QPSK or pi/2-BPSK, as per TS38.211 Section 6.3.2.6.2.
  modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;

  // Get a boolean mask of the OFDM symbols carrying DM-RS.
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);

  // Number of REs per OFDM symbol.
  const unsigned nof_re_symb = config.nof_prb * NRE;

  // Number of data Resource Elements in a slot for a single Rx port.
  const unsigned nof_re_port = (config.nof_symbols - dmrs_symb_mask.count()) * nof_re_symb;

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
                    ((*config.second_hop_prb + config.nof_prb) * NRE <= grid.get_nof_subc()),
                "PUCCH Format 3: PRB allocation outside grid (second hop). Requested [{}, {}), grid has {} PRBs.",
                *config.second_hop_prb,
                *config.second_hop_prb + config.nof_prb,
                grid.get_nof_subc() / NRE);

  interval<unsigned, true> nof_symbols_range(pucch_constants::FORMAT3_MIN_NSYMB, pucch_constants::FORMAT3_MAX_NSYMB);
  srsran_assert(nof_symbols_range.contains(config.nof_symbols),
                "Invalid Number of OFDM symbols allocated to PUCCH Format 3, i.e., {}. Valid range is {}.",
                config.nof_symbols,
                nof_symbols_range);

  // Resize equalized data and post equalization noise variance buffers.
  eq_re.resize(nof_re_port);
  eq_noise_vars.resize(nof_re_port);

  // Assert that the number of RE returned by the channel equalizer matches the expected number of LLR.
  srsran_assert(eq_re.size() == llr.size() / get_bits_per_symbol(mod_scheme),
                "Number of equalized RE (i.e. {}) does not match the expected LLR data length (i.e. {})",
                eq_re.size(),
                llr.size() / get_bits_per_symbol(mod_scheme));

  pucch_3_4_extract_and_equalize(eq_re,
                                 eq_noise_vars,
                                 *equalizer,
                                 *precoder,
                                 grid,
                                 estimates,
                                 dmrs_symb_mask,
                                 config.start_symbol_index,
                                 config.nof_symbols,
                                 config.nof_prb,
                                 config.first_prb,
                                 config.second_hop_prb,
                                 config.rx_ports);

  // Apply soft symbol demodulation.
  demapper->demodulate_soft(llr, eq_re, eq_noise_vars, mod_scheme);

  // Descramble, as per TS38.211 Section 6.3.2.6.1.
  const unsigned c_init = (config.rnti * pow2(15)) + config.n_id;
  descrambler->init(c_init);
  descrambler->apply_xor(llr, llr);
}
