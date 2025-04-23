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
/// \brief PUCCH Format 2 demodulator definition.

#include "pucch_demodulator_format2.h"
#include "srsran/phy/support/mask_types.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/srsvec/copy.h"

#if defined(__AVX2__)
#include <immintrin.h>
#elif defined(__ARM_NEON)
#include <arm_neon.h>
#endif

using namespace srsran;

/// \brief Extracts REs containing PUCCH Format 2 data from a block of contiguous PRBs.
/// \param[out] out Output buffer.
/// \param[in]  in  Input buffer containing data and DM-RS.
static inline void extract_re(span<cbf16_t> out, span<const cbf16_t> in)
{
  srsran_assert(in.size() % NRE == 0, "Invalid output size.");
  unsigned nof_prb = in.size() / NRE;
  srsran_assert(out.size() == nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC,
                "Invalid output size (i.e., {}), expected {}.",
                out.size(),
                nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC);

#if defined(__AVX2__)
  const int* in_ptr   = reinterpret_cast<const int*>(in.data());
  __m256i*   out_ptr  = reinterpret_cast<__m256i*>(out.data());
  __m256i    data_idx = _mm256_setr_epi32(0, 2, 3, 5, 6, 8, 9, 11);

  for (unsigned i_prb = 0; i_prb != nof_prb; ++i_prb) {
    // Gather data RE as 32-bit unsigned integers.
    __m256i avx2_data = _mm256_i32gather_epi32(in_ptr, data_idx, 4);
    in_ptr += NRE;

    // Store data REs.
    _mm256_storeu_si256(out_ptr++, avx2_data);
  }
#elif defined(__ARM_NEON)
  const int* in_ptr  = reinterpret_cast<const int*>(in.data());
  int*       out_ptr = reinterpret_cast<int*>(out.data());

  for (unsigned i_prb = 0; i_prb != nof_prb; ++i_prb) {
    // Load and deinterleave 12 REs as 32-bit unsigned integers.
    int32x4x3_t data_prb_s32 = vld3q_s32(in_ptr);
    in_ptr += NRE;

    // Discard second RE.
    int32x4x2_t data_s32 = {data_prb_s32.val[0], data_prb_s32.val[2]};

    // Interleave and store.
    vst2q_s32(out_ptr, data_s32);
    out_ptr += pucch_constants::FORMAT2_NOF_DATA_SC;
  }
#else
  for (unsigned k = 0, k_end = nof_prb * NRE, count = 0; k != k_end; ++k) {
    // Skip DM-RS.
    if (k % 3 == 1) {
      continue;
    }

    out[count++] = in[k];
  }
#endif
}

void pucch_demodulator_format2::demodulate(span<log_likelihood_ratio>                      llr,
                                           const resource_grid_reader&                     grid,
                                           const channel_estimate&                         estimates,
                                           const pucch_demodulator::format2_configuration& config)
{
  // Number of receive antenna ports.
  auto nof_rx_ports = static_cast<unsigned>(config.rx_ports.size());

  // Number of data Resource Elements in a slot for a single Rx port.
  unsigned nof_re_port = pucch_constants::FORMAT2_NOF_DATA_SC * config.nof_prb * config.nof_symbols;

  // Assert that allocations are valid.
  srsran_assert(config.nof_prb && config.nof_prb <= pucch_constants::FORMAT2_MAX_NPRB,
                "Invalid Number of PRB allocated to PUCCH Format 2, i.e., {}. Valid range is 1 to {}.",
                config.nof_prb,
                pucch_constants::FORMAT2_MAX_NPRB);

  srsran_assert((config.first_prb + config.nof_prb) * NRE <= grid.get_nof_subc(),
                "PUCCH Format 2: PRB allocation outside grid (first hop). Requested [{}, {}), grid has {} PRBs.",
                config.first_prb,
                config.first_prb + config.nof_prb,
                grid.get_nof_subc() / NRE);

  srsran_assert(!config.second_hop_prb.has_value() ||
                    ((*config.second_hop_prb + config.nof_prb) * NRE <= grid.get_nof_subc()),
                "PUCCH Format 2: PRB allocation outside grid (second hop). Requested [{}, {}), grid has {} PRBs.",
                *config.second_hop_prb,
                *config.second_hop_prb + config.nof_prb,
                grid.get_nof_subc() / NRE);

  srsran_assert(config.nof_symbols && config.nof_symbols <= pucch_constants::FORMAT2_MAX_NSYMB,
                "Invalid Number of OFDM symbols allocated to PUCCH Format 2, i.e., {}. Valid range is 1 to {}.",
                config.nof_symbols,
                pucch_constants::FORMAT2_MAX_NSYMB);

  // Resize data and channel estimation buffers.
  ch_re.resize(nof_rx_ports, nof_re_port);
  ch_estimates.resize(nof_re_port, nof_rx_ports, pucch_constants::MAX_LAYERS);

  // Resize equalized data and post equalization noise variance buffers.
  eq_re.resize(nof_re_port);
  eq_noise_vars.resize(nof_re_port);

  // Extract data RE and channel estimation coefficients.
  get_data_re_ests(grid, estimates, config);

  // Extract the Rx port noise variances from the channel estimation.
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    noise_var_estimates[i_port] = estimates.get_noise_variance(i_port);
  }

  // Equalize the data RE.
  equalizer->equalize(
      eq_re, eq_noise_vars, ch_re, ch_estimates, span<float>(noise_var_estimates).first(nof_rx_ports), 1.0F);

  // Assert that the number of RE returned by the channel equalizer matches the expected number of LLR.
  srsran_assert(eq_re.size() == llr.size() / get_bits_per_symbol(modulation_scheme::QPSK),
                "Number of equalized RE (i.e. {}) does not match the expected LLR data length (i.e. {})",
                eq_re.size(),
                llr.size() / get_bits_per_symbol(modulation_scheme::QPSK));

  // Apply soft symbol demodulation. PUCCH Format 2 modulation scheme is always QPSK, as per TS38.211 Section 6.3.2.5.2.
  demapper->demodulate_soft(llr, eq_re, eq_noise_vars, modulation_scheme::QPSK);

  // Descramble, as per TS38.211 Section 6.3.2.5.1.
  unsigned c_init = config.rnti * pow2(15) + config.n_id;
  descrambler->init(c_init);
  descrambler->apply_xor(llr, llr);
}

void pucch_demodulator_format2::get_data_re_ests(const resource_grid_reader&                     resource_grid,
                                                 const channel_estimate&                         channel_ests,
                                                 const pucch_demodulator::format2_configuration& config)
{
  for (unsigned i_port = 0, i_port_end = config.rx_ports.size(); i_port != i_port_end; ++i_port) {
    // Get a view of the data RE destination buffer for a single Rx port.
    span<cbf16_t> re_port_buffer = ch_re.get_slice(i_port);

    // Get a view of the channel estimates destination buffer for a single Rx port and Tx layer.
    span<cbf16_t> ests_port_buffer = ch_estimates.get_channel(i_port, 0);

    // First OFDM subcarrier containing PUCCH Format 2.
    unsigned first_subc = config.first_prb * NRE;

    for (unsigned i_symbol = config.start_symbol_index, i_symbol_end = config.start_symbol_index + config.nof_symbols;
         i_symbol != i_symbol_end;
         ++i_symbol) {
      if ((i_symbol > config.start_symbol_index) && config.second_hop_prb.has_value()) {
        first_subc = *config.second_hop_prb * NRE;
      }

      // Extract data RE view from the resource grid.
      span<const cbf16_t> grid_view =
          resource_grid.get_view(i_port, i_symbol).subspan(first_subc, config.nof_prb * NRE);

      // View over the channel estimation coefficients for a single OFDM symbol.
      span<const cbf16_t> ests_symbol =
          channel_ests.get_symbol_ch_estimate(i_symbol, i_port).subspan(first_subc, config.nof_prb * NRE);

      // Extract data resource elements.
      extract_re(re_port_buffer.first(config.nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC), grid_view);
      extract_re(ests_port_buffer.first(config.nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC), ests_symbol);

      // Advance buffers.
      re_port_buffer =
          re_port_buffer.last(re_port_buffer.size() - config.nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC);
      ests_port_buffer =
          ests_port_buffer.last(ests_port_buffer.size() - config.nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC);
    }

    // Assert that all port data RE buffer elements have been filled.
    srsran_assert(re_port_buffer.empty(),
                  "Number of extracted port data RE does not match destination buffer dimensions: "
                  "{} unused elements.",
                  re_port_buffer.size());

    // Assert that all port channel estimates buffer elements have been filled.
    srsran_assert(
        ests_port_buffer.empty(),
        "Number of extracted port channel estimation coefficients does not match destination buffer dimensions: "
        "{} unused elements.",
        ests_port_buffer.size());
  }
}
