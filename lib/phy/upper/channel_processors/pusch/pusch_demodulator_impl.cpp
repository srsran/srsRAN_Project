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
#include "srsran/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator_notifier.h"
#include "srsran/ran/sch_dmrs_power.h"
#include "srsran/srsvec/mean.h"

#if defined(__SSE3__)
#include <immintrin.h>
#elif defined(__aarch64__)
#include <arm_neon.h>
#endif

using namespace srsran;

static void
revert_scrambling(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in, const bit_buffer& sequence)
{
  srsran_assert(in.size() == out.size(),
                "Input size (i.e., {}) and output size (i.e., {}) must be equal.",
                in.size(),
                out.size());

  unsigned i      = 0;
  unsigned length = in.size();

#ifdef __SSE3__
  // Number of bits that can be processed with a SIMD register.
  static constexpr unsigned nof_bits_per_simd = 16;

  for (unsigned i_byte = 0, i_end = (length / nof_bits_per_simd) * nof_bits_per_simd; i != i_end;
       i_byte += 2, i += nof_bits_per_simd) {
    uint8_t byte0 = sequence.get_byte(i_byte);
    uint8_t byte1 = sequence.get_byte(i_byte + 1);
    int32_t c     = static_cast<int32_t>(byte0) + (static_cast<int32_t>(byte1) << 8);

    // Preload bits of interest in the 16 LSB.
    __m128i mask = _mm_set1_epi32(c);
    mask         = _mm_shuffle_epi8(mask, _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1));

    // Mask each bit.
    mask = _mm_and_si128(mask, _mm_set_epi64x(0x0102040810204080, 0x0102040810204080));

    // Get non zero mask.
    mask = _mm_cmpeq_epi8(mask, _mm_set_epi64x(0x0102040810204080, 0x0102040810204080));

    // Load input.
    __m128i v = _mm_loadu_si128((const __m128i*)(&in[i]));

    // Negate.
    v = _mm_xor_si128(mask, v);

    // Add one.
    mask = _mm_and_si128(mask, _mm_set1_epi8(1));
    v    = _mm_add_epi8(v, mask);

    _mm_storeu_si128((__m128i*)(&out[i]), v);
  }
#endif // __SSE3__

#ifdef __aarch64__
  // Number of bits that can be processed with a SIMD register.
  static constexpr unsigned nof_bits_per_simd = 16;

  for (unsigned i_byte = 0, i_end = (length / nof_bits_per_simd) * nof_bits_per_simd; i != i_end;
       i_byte += 2, i += nof_bits_per_simd) {
    uint8_t byte0 = sequence.get_byte(i_byte);
    uint8_t byte1 = sequence.get_byte(i_byte + 1);
    int32_t c     = static_cast<int32_t>(byte0) + (static_cast<int32_t>(byte1) << 8);

    // Preload bits of interest in the 16 LSB.
    uint32x2_t c_dup_u32 = vdup_n_u32(c);
    uint8x16_t mask_u8 =
        vcombine_u8(vdup_lane_u8(vreinterpret_u8_u32(c_dup_u32), 0), vdup_lane_u8(vreinterpret_u8_u32(c_dup_u32), 1));

    // Create bit masks.
    const uint8_t    bit_masks[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
    const uint8x16_t bit_masks_u8 = vcombine_u8(vcreate_u8(*(reinterpret_cast<const uint64_t*>(bit_masks))),
                                                vcreate_u8(*(reinterpret_cast<const uint64_t*>(bit_masks))));
    // Mask each bit.
    mask_u8 = vandq_u8(mask_u8, bit_masks_u8);

    // Get non zero mask.
    mask_u8 = vceqq_u8(mask_u8, bit_masks_u8);

    // Load input.
    int8x16_t v = vld1q_s8(reinterpret_cast<const int8_t*>(&in[i]));

    // Negate.
    v = veorq_s8(vreinterpretq_s8_u8(mask_u8), v);

    // Add one.
    int8x16_t one_s8 = vandq_s8(vreinterpretq_s8_u8(mask_u8), vdupq_n_s8(1));
    v                = vaddq_s8(v, one_s8);

    // Store the result.
    vst1q_s8(reinterpret_cast<int8_t*>(&out[i]), v);
  }
#endif // __aarch64__

  // Apply remaining bits.
  for (; i != length; ++i) {
    out[i] = in[i].to_value_type() * ((sequence.extract(i, 1) == 1) ? -1 : 1);
  }
}

void pusch_demodulator_impl::demodulate(pusch_codeword_buffer&      codeword_buffer,
                                        pusch_demodulator_notifier& notifier,
                                        const resource_grid_reader& grid,
                                        const channel_estimate&     estimates,
                                        const configuration&        config)
{
  // Number of receive antenna ports.
  unsigned nof_rx_ports = static_cast<unsigned>(config.rx_ports.size());

  // Initialise sequence.
  unsigned c_init = config.rnti * pow2(15) + config.n_id;
  descrambler->init(c_init);

  // Prepare PRB active RE mask.
  re_prb_mask active_re_per_prb      = ~re_prb_mask();
  re_prb_mask active_re_per_prb_dmrs = ~config.dmrs_config_type.get_dmrs_prb_mask(config.nof_cdm_groups_without_data);

  // Prepare RE mask.
  re_symbol_mask_type re_mask      = config.rb_mask.kronecker_product<NRE>(active_re_per_prb);
  re_symbol_mask_type re_mask_dmrs = config.rb_mask.kronecker_product<NRE>(active_re_per_prb_dmrs);

  // Calculate the number of bits per RE and port.
  unsigned nof_bits_per_re = config.nof_tx_layers * get_bits_per_symbol(config.modulation);
  srsran_assert(nof_bits_per_re > 0,
                "Invalid combination of transmit layers (i.e., {}) and modulation (i.e., {}).",
                config.nof_tx_layers,
                to_string(config.modulation));

  // Stats accumulators.
  pusch_demodulator_notifier::demodulation_stats stats                = {};
  unsigned                                       evm_symbol_count     = 0;
  unsigned                                       sinr_softbit_count   = 0;
  float                                          noise_var_accumulate = 0.0;
  float                                          evm_accumulate       = 0.0;

  for (unsigned i_symbol = config.start_symbol_index, i_symbol_end = config.start_symbol_index + config.nof_symbols;
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // Select RE mask for the symbol.
    re_symbol_mask_type& symbol_re_mask = config.dmrs_symb_pos[i_symbol] ? re_mask_dmrs : re_mask;

    // Skip symbol if it does not contain data.
    if (symbol_re_mask.none()) {
      continue;
    }

    // Process subcarriers in groups.
    for (unsigned i_subc = symbol_re_mask.find_lowest(), i_subc_end = symbol_re_mask.find_highest() + 1;
         i_subc != i_subc_end;) {
      // Calculate the remainder number of subcarriers to process for the current OFDM symbol.
      unsigned nof_block_subc = i_subc_end - i_subc;

      // Limit number of RE to the maximum block size.
      nof_block_subc = std::min(nof_block_subc, MAX_BLOCK_SIZE / nof_bits_per_re);

      // Get a view of the codeword buffer destination.
      span<log_likelihood_ratio> codeword_block = codeword_buffer.get_next_block_view(nof_block_subc * nof_bits_per_re);

      // If the codeword is empty, skip the rest of the symbol.
      if (codeword_block.empty()) {
        unsigned nof_remainder_re = symbol_re_mask.slice(i_subc, symbol_re_mask.size()).count();
        srsran_assert(nof_remainder_re == 0, "There are {} remaining RE.", nof_remainder_re);
        break;
      }

      // Limit block size if the codeword block is smaller.
      nof_block_subc = std::min(nof_block_subc, static_cast<unsigned>(codeword_block.size()) / nof_bits_per_re);

      // Extract mask for the block.
      re_symbol_mask_type block_re_mask = symbol_re_mask.slice(i_subc, i_subc + nof_block_subc);

      // Number of data Resource Elements in a slot for a single Rx port.
      unsigned nof_re_port = static_cast<unsigned>(block_re_mask.count());

      // Skip block if no data.
      if (nof_re_port == 0) {
        i_subc += nof_block_subc;
        continue;
      }

      // Resize equalizer input buffers.
      ch_re.resize({nof_re_port, nof_rx_ports});
      ch_estimates.resize({nof_re_port, nof_rx_ports, config.nof_tx_layers});

      // Resize equalizer output buffers.
      eq_re.resize({nof_re_port, config.nof_tx_layers});
      eq_noise_vars.resize({nof_re_port, config.nof_tx_layers});

      // Extract the data symbols and channel estimates from the resource grid.
      get_ch_data_re(ch_re, grid, i_symbol, i_subc, block_re_mask, config.rx_ports);
      get_ch_data_estimates(ch_estimates, estimates, i_symbol, i_subc, block_re_mask, config.rx_ports);

      // Increment subcarrier count.
      i_subc += nof_block_subc;

      // Extract the Rx port noise variances from the channel estimation.
      for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
        noise_var_estimates[i_port] = estimates.get_noise_variance(i_port, 0);
      }

      // Equalize channels and, for each Tx layer, combine contribution from all Rx antenna ports.
      equalizer->equalize(
          eq_re, eq_noise_vars, ch_re, ch_estimates, span<float>(noise_var_estimates).first(nof_rx_ports), 1.0F);

      // Estimate post equalization Signal-to-Interference-plus-Noise Ratio.
      if (compute_post_eq_sinr) {
        span<const float> all_eq_noise_vars = eq_noise_vars.get_data();
        noise_var_accumulate += std::accumulate(
            all_eq_noise_vars.begin(), all_eq_noise_vars.end(), 0.0F, [&sinr_softbit_count](float sum, float in) {
              // Exclude outliers with infinite variance. This makes sure that handling of the DC carrier does not skew
              // the SINR results.
              if (std::isinf(in)) {
                return sum;
              }

              ++sinr_softbit_count;
              return sum + in;
            });
      }

      // For now, layer demapping is not implemented.
      srsran_assert(config.nof_tx_layers == 1, "Only a single transmit layer is supported.");

      // Get the equalized resource elements and noise variances for a single transmit layer.
      span<const cf_t>  eq_re_flat   = eq_re.get_view({0});
      span<const float> eq_vars_flat = eq_noise_vars.get_view({0});

      // Get codeword buffer.
      unsigned                   nof_block_softbits = nof_re_port * nof_bits_per_re;
      span<log_likelihood_ratio> codeword           = codeword_block.first(nof_block_softbits);

      // Build LLRs from channel symbols.
      demapper->demodulate_soft(codeword, eq_re_flat, eq_vars_flat, config.modulation);

      // Calculate EVM only if it is available.
      if (evm_calc) {
        unsigned nof_re_evm = eq_re_flat.size();
        evm_accumulate += static_cast<float>(nof_re_evm) * evm_calc->calculate(codeword, eq_re_flat, config.modulation);
        evm_symbol_count += nof_re_evm;
      }

      // Update and notify statistics.
      if (i_subc == i_subc_end) {
        if (sinr_softbit_count != 0) {
          if (noise_var_accumulate > 0.0) {
            float mean_noise_var = noise_var_accumulate / static_cast<float>(sinr_softbit_count);
            stats.sinr_dB.emplace(-convert_power_to_dB(mean_noise_var));
          }
        }
        if (evm_symbol_count != 0) {
          stats.evm.emplace(evm_accumulate / static_cast<float>(evm_symbol_count));
        }
        notifier.on_provisional_stats(stats);
      }

      // Generate scrambling sequence.
      static_bit_buffer<MAX_BLOCK_SIZE> temp_scrambling_seq(nof_block_softbits);
      bit_buffer                        scrambling_seq = temp_scrambling_seq;
      descrambler->generate(scrambling_seq);

      // Revert scrambling.
      revert_scrambling(codeword, codeword, scrambling_seq);

      // Notify a new processed block.
      codeword_buffer.on_new_block(codeword, scrambling_seq);
    }
  }

  notifier.on_end_stats(stats);
  codeword_buffer.on_end_codeword();
}
