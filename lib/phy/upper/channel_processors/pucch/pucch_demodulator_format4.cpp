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
/// \brief PUCCH Format 4 demodulator definition.

#include "pucch_demodulator_format4.h"
#include "srsran/phy/support/mask_types.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/pucch_formats3_4_helpers.h"
#include "srsran/phy/upper/pucch_orthogonal_sequence.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

void pucch_demodulator_format4::demodulate(span<log_likelihood_ratio>                      llr,
                                           const resource_grid_reader&                     grid,
                                           const channel_estimate&                         estimates,
                                           const pucch_demodulator::format4_configuration& config)
{
  // PUCCH Format 4 modulation scheme can be QPSK or pi/2-BPSK, as per TS38.211 Section 6.3.2.6.2.
  modulation_scheme mod_scheme = config.pi2_bpsk ? modulation_scheme::PI_2_BPSK : modulation_scheme::QPSK;

  // Get a boolean mask of the OFDM symbols carrying DM-RS.
  symbol_slot_mask dmrs_symb_mask = get_pucch_formats3_4_dmrs_symbol_mask(
      config.nof_symbols, config.second_hop_prb.has_value(), config.additional_dmrs);

  // Number of REs per OFDM symbol. PUCCH Format 4 only gets a PRB.
  unsigned nof_re_symb = NRE;

  // Number of data Resource Elements in a slot for a single Rx port.
  unsigned nof_re_port = (config.nof_symbols - dmrs_symb_mask.count()) * nof_re_symb;

  // Assert that allocations are valid.
  srsran_assert(config.first_prb * NRE <= grid.get_nof_subc(),
                "PUCCH Format 4: PRB allocation outside grid (first hop). Requested {}, grid has {} PRBs.",
                config.first_prb,
                grid.get_nof_subc() / NRE);

  srsran_assert(!config.second_hop_prb.has_value() || (*config.second_hop_prb * NRE <= grid.get_nof_subc()),
                "PUCCH Format 4: PRB allocation outside grid (second hop). Requested {}, grid has {} PRBs.",
                *config.second_hop_prb,
                grid.get_nof_subc() / NRE);

  interval<unsigned, true> nof_symbols_range(pucch_constants::FORMAT4_MIN_NSYMB, pucch_constants::FORMAT4_MAX_NSYMB);
  srsran_assert(nof_symbols_range.contains(config.nof_symbols),
                "Invalid Number of OFDM symbols allocated to PUCCH Format 4, i.e., {}. Valid range is {}.",
                config.nof_symbols,
                nof_symbols_range);

  // Resize equalized data and post equalization noise variance buffers.
  eq_re.resize(nof_re_port);
  eq_noise_vars.resize(nof_re_port);

  // Assert that the number of RE returned by the channel equalizer matches the expected number of LLR.
  srsran_assert(eq_re.size() / config.occ_length == llr.size() / get_bits_per_symbol(mod_scheme),
                "Number of equalized REs (i.e. {}) does not match the expected LLR data length (i.e. {})",
                eq_re.size() / config.occ_length,
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
                                 1,
                                 config.first_prb,
                                 config.second_hop_prb,
                                 config.rx_ports);

  // Create vectors to hold the output of the inverse blockwise spreading.
  static_vector<cf_t, pucch_constants::MAX_NOF_RE>  original(eq_re.size() / config.occ_length, cf_t());
  static_vector<float, pucch_constants::MAX_NOF_RE> noise_vars(original.size(), 0.0f);

  // Inverse block-wise spreading, as per TS38.211 Section 6.3.2.6.3.
  inverse_blockwise_spreading(original, noise_vars, eq_re, eq_noise_vars, config);

  // Apply soft symbol demodulation.
  demapper->demodulate_soft(llr, original, noise_vars, mod_scheme);

  // Descramble, as per TS38.211 Section 6.3.2.6.1.
  unsigned c_init = (config.rnti * pow2(15)) + config.n_id;
  descrambler->init(c_init);
  descrambler->apply_xor(llr, llr);
}

void pucch_demodulator_format4::inverse_blockwise_spreading(span<cf_t>        original,
                                                            span<float>       noise_vars,
                                                            span<const cf_t>  eq_re,
                                                            span<const float> eq_noise_vars,
                                                            const pucch_demodulator::format4_configuration& config)
{
  unsigned         mod = 12 / config.occ_length;
  span<const cf_t> wn  = pucch_orthogonal_sequence_format4::get_sequence(config.occ_length, config.occ_index);

  for (unsigned k = 0; k != NRE; ++k) {
    for (unsigned l = 0, l_end = eq_re.size() / NRE; l != l_end; ++l) {
      unsigned original_index = (l * mod) + (k % mod);
      unsigned spread_index   = (l * 12) + k;
      original[original_index] += eq_re[spread_index] / wn[k];
      noise_vars[original_index] += eq_noise_vars[spread_index];
    }
  }

  // Scale according to the spreading factor.
  srsvec::sc_prod(original, original, 1.0F / config.occ_length);
}
