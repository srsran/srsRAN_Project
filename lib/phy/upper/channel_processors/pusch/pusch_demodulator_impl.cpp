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
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator_notifier.h"
#include "srsran/ran/sch_dmrs_power.h"
#include "srsran/srsvec/mean.h"

using namespace srsran;

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
  bounded_bitset<MAX_RB* NRE> re_mask      = config.rb_mask.kronecker_product<NRE>(active_re_per_prb);
  bounded_bitset<MAX_RB* NRE> re_mask_dmrs = config.rb_mask.kronecker_product<NRE>(active_re_per_prb_dmrs);

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
    bounded_bitset<MAX_RB* NRE>& symbol_re_mask = config.dmrs_symb_pos[i_symbol] ? re_mask_dmrs : re_mask;

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
      bounded_bitset<NRE* MAX_RB> block_re_mask = symbol_re_mask.slice(i_subc, i_subc + nof_block_subc);

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
        noise_var_accumulate += std::accumulate(all_eq_noise_vars.begin(), all_eq_noise_vars.end(), 0.0F);
        sinr_softbit_count += all_eq_noise_vars.size();
      }

      // For now, layer demapping is not implemented.
      srsran_assert(config.nof_tx_layers == 1, "Only a single transmit layer is supported.");

      // Get the equalized resource elements and noise variances for a single transmit layer.
      span<const cf_t>  eq_re_flat   = eq_re.get_view({0});
      span<const float> eq_vars_flat = eq_noise_vars.get_view({0});

      // Get codeword buffer. Use codeword_buffer.get_next_block_view(codeword_size) in next revision.
      unsigned                   nof_block_softbits = nof_re_port * nof_bits_per_re;
      span<log_likelihood_ratio> codeword = span<log_likelihood_ratio>(temp_codeword).first(nof_block_softbits);

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

      // Process remaining data.
      span<log_likelihood_ratio> codeword_descr = codeword_block.first(nof_block_softbits);
      descrambler->apply_xor(codeword_descr, codeword);

      // Currently the codeword is processed in a single block and after scrambling.
      codeword_buffer.on_new_block(codeword, codeword_descr);
    }
  }

  notifier.on_end_stats(stats);
  codeword_buffer.on_end_codeword();
}
