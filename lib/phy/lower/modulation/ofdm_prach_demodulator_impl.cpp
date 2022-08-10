/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofdm_prach_demodulator_impl.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/ran/prach/prach_frequency_mapping.h"
#include "srsgnb/ran/prach/prach_preamble_information.h"
#include "srsgnb/srsvec/copy.h"

using namespace srsgnb;

void ofdm_prach_demodulator_impl::demodulate(prach_buffer&                                buffer,
                                             span<const cf_t>                             input,
                                             const ofdm_prach_demodulator::configuration& config)
{
  srsgnb_assert(config.format.is_long_preamble(), "Only PRACH long preamble formats are implemented.");

  prach_preamble_information          preamble_info = get_prach_preamble_long_info(config.format);
  prach_frequency_mapping_information freq_mapping_info =
      prach_frequency_mapping_get(preamble_info.scs, config.pusch_scs);
  srsgnb_assert(freq_mapping_info.nof_rb_ra != PRACH_FREQUENCY_MAPPING_INFORMATION_RESERVED.nof_rb_ra,
                "The PRACH and PUSCH subcarrier spacing combination resulted in a reserved configuration.");
  srsgnb_assert(freq_mapping_info.k_bar != PRACH_FREQUENCY_MAPPING_INFORMATION_RESERVED.k_bar,
                "The PRACH and PUSCH subcarrier spacing combination resulted in a reserved configuration.");

  dft_processor& dft              = (preamble_info.scs == prach_subcarrier_spacing::kHz1_25) ? *dft_1_25kHz : *dft_5kHz;
  unsigned       prach_scs_Hz     = preamble_info.scs.to_Hz();
  unsigned       pusch_scs_Hz     = scs_to_khz(config.pusch_scs) * 1000;
  unsigned       ofdm_symbol_len  = dft.get_size();
  unsigned       sampling_rate_Hz = ofdm_symbol_len * prach_scs_Hz;
  unsigned       time_sequence_length = preamble_info.symbol_length.to_samples(sampling_rate_Hz);
  unsigned       cyclic_prefix_length = preamble_info.cp_length.to_samples(sampling_rate_Hz);
  unsigned       nof_symbols          = time_sequence_length / ofdm_symbol_len;

  // Make sure the input size matches the PRACH receive window.
  srsgnb_assert(input.size() >= time_sequence_length + cyclic_prefix_length,
                "The number of input samples {} is lesser than the PRACH receive window length {}.",
                input.size(),
                time_sequence_length + cyclic_prefix_length);

  // Prepare PRACH resource grid.
  unsigned K               = pusch_scs_Hz / prach_scs_Hz;
  unsigned prach_grid_size = config.nof_prb_ul_grid * K * NRE;
  srsgnb_assert(
      dft.get_size() > prach_grid_size,
      "DFT size {} for PRACH SCS {} is not sufficient for K={}, N_RB={}. The minimum expected DFT size is {}.",
      dft.get_size(),
      prach_scs_Hz,
      K,
      config.nof_prb_ul_grid,
      prach_grid_size);

  // Calculate initial subcarrier.
  unsigned k_start = K * NRE * config.rb_offset + freq_mapping_info.k_bar;
  srsgnb_assert(k_start + preamble_info.sequence_length < prach_grid_size,
                "Start subcarrier {} plus sequence length {} exceeds PRACH grid size {}.",
                k_start,
                preamble_info.sequence_length,
                prach_grid_size);

  // Skip cyclic prefix.
  input = input.last(input.size() - cyclic_prefix_length);

  // Process each PRACH symbol.
  for (unsigned symbol = 0; symbol != nof_symbols; ++symbol) {
    // Load symbol time-domain in the DFT.
    srsvec::copy(dft.get_input(), input.first(ofdm_symbol_len));

    // Perform DFT.
    span<const cf_t> dft_output = dft.run();

    // Select destination PRACH symbol in the buffer.
    span<cf_t> prach_symbol = buffer.get_symbol(symbol);

    // Create views of the lower and upper grid.
    span<const cf_t> lower_grid = dft_output.last(prach_grid_size / 2);
    span<const cf_t> upper_grid = dft_output.first(prach_grid_size / 2);

    // If the sequence map starts at the lower half of the frequency band.
    if (k_start < prach_grid_size / 2) {
      // Number of subcarrier to fill in the lower half of the grid.
      unsigned N = std::min(prach_grid_size / 2 - k_start, preamble_info.sequence_length);

      // Copy first N subcarriers of the sequence in the lower half grid.
      srsvec::copy(prach_symbol.first(N), lower_grid.subspan(k_start, N));

      // Copy the remainder of the sequence in the upper half grid.
      srsvec::copy(prach_symbol.last(preamble_info.sequence_length - N),
                   upper_grid.first(preamble_info.sequence_length - N));
    } else {
      // Copy the sequence in the upper half grid.
      srsvec::copy(prach_symbol, upper_grid.subspan(k_start - prach_grid_size / 2, preamble_info.sequence_length));
    }

    // Advance input.
    input = input.last(input.size() - ofdm_symbol_len);
  }
}