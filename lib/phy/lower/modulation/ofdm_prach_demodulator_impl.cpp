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

#include "ofdm_prach_demodulator_impl.h"
#include "srsran/phy/constants.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;

void ofdm_prach_demodulator_impl::demodulate(prach_buffer&                                buffer,
                                             span<const cf_t>                             input,
                                             const ofdm_prach_demodulator::configuration& config)
{
  // Cyclic prefix extension for short preambles at 0 and 0.5 ms from the start of the subframe.
  static constexpr phy_time_unit sixteen_kappa = phy_time_unit::from_units_of_kappa(16);

  // PUSCH symbol duration.
  phy_time_unit pusch_symbol_duration =
      phy_time_unit::from_units_of_kappa((144U + 2048U) >> to_numerology_value(config.pusch_scs));

  // Validate configuration.
  if (is_short_preamble(config.format)) {
    unsigned prach_window_length =
        get_prach_window_duration(config.format, config.pusch_scs, config.start_symbol, config.nof_td_occasions)
            .to_samples(srate.to_Hz());
    srsran_assert(
        input.size() >= prach_window_length,
        "The number of input samples (i.e., {}) must be equal to or greater than the PRACH window (i.e., {}).",
        input.size(),
        prach_window_length);
    srsran_assert(config.nof_td_occasions > 0, "The number of time-domain occasions must be greater than 0.");
  } else {
    srsran_assert(config.nof_td_occasions == 1, "Long preambles only support one occasion.");
  }
  srsran_assert(config.nof_td_occasions <= buffer.get_max_nof_td_occasions(),
                "The number of time-domain occasions (i.e., {}) exceeds the buffer maximum number of "
                "time-domain occasions (i.e., {}).",
                config.nof_td_occasions,
                buffer.get_max_nof_td_occasions());
  srsran_assert(config.nof_fd_occasions <= buffer.get_max_nof_fd_occasions(),
                "The number of frequency-domain occasions (i.e., {}) exceeds the buffer maximum number of "
                "frequency-domain occasions (i.e., {}).",
                config.nof_fd_occasions,
                buffer.get_max_nof_fd_occasions());
  srsran_assert(config.port < buffer.get_max_nof_ports(),
                "The port identifier (i.e., {}) exceeds the maximum number of PRACH buffer ports (i.e., {}).",
                config.port,
                buffer.get_max_nof_ports());

  for (unsigned i_td_occasion = 0; i_td_occasion != config.nof_td_occasions; ++i_td_occasion) {
    bool is_last_occasion = (i_td_occasion == (config.nof_td_occasions - 1));

    // Get preamble information for the time occasion.
    prach_preamble_information preamble_info =
        is_long_preamble(config.format) ? get_prach_preamble_long_info(config.format)
                                        : get_prach_preamble_short_info(config.format,
                                                                        to_ra_subcarrier_spacing(config.pusch_scs),
                                                                        is_last_occasion);

    unsigned pusch_scs_Hz = scs_to_khz(config.pusch_scs) * 1000;

    // Calculate time-domain occasion symbol start.
    unsigned t_occasion_start_symbol = config.start_symbol + get_preamble_duration(config.format) * i_td_occasion;

    // Calculate time-domain occasion start time.
    phy_time_unit t_occasion_start = pusch_symbol_duration * t_occasion_start_symbol;

    // Add sixteen kappa units if the symbol doesn't start at the beginning of the slot.
    if (t_occasion_start > phy_time_unit::from_seconds(0.0)) {
      t_occasion_start += sixteen_kappa;
    }

    // Add sixteen kappa units if the symbol starts more than 0.5 ms after the beginning of the slot.
    if (t_occasion_start > phy_time_unit::from_seconds(0.5e-3)) {
      t_occasion_start += sixteen_kappa;
    }

    // Calculate time-domain occasion end time.
    phy_time_unit t_occasion_end = t_occasion_start + preamble_info.cp_length + preamble_info.symbol_length;

    // Add sixteen kappa to the cyclic prefix length if ...
    if (is_short_preamble(preamble_info.scs)) {
      // The occasion overlaps with time zero.
      if ((t_occasion_start <= phy_time_unit::from_seconds(0.0)) &&
          (t_occasion_end >= phy_time_unit::from_seconds(0.0))) {
        preamble_info.cp_length += sixteen_kappa;
      }
      // The occasion overlaps with time 0.5ms from the beginning of the slot.
      if ((t_occasion_start <= phy_time_unit::from_seconds(0.5e-3)) &&
          (t_occasion_end >= phy_time_unit::from_seconds(0.5e-3))) {
        preamble_info.cp_length += sixteen_kappa;
      }
    }

    // Calculate occasion duration.
    phy_time_unit occasion_duration = preamble_info.cp_length + preamble_info.symbol_length;

    unsigned sample_offset = t_occasion_start.to_samples(srate.to_Hz());
    unsigned nof_samples   = occasion_duration.to_samples(srate.to_Hz());

    // Select view of the input samples for the occasion.
    span<const cf_t> input_occasion = input.subspan(sample_offset, nof_samples);

    // Get frequency mapping information, common for all frequency-domain occasions.
    prach_frequency_mapping_information freq_mapping_info =
        prach_frequency_mapping_get(preamble_info.scs, config.pusch_scs);
    srsran_assert(freq_mapping_info.nof_rb_ra != PRACH_FREQUENCY_MAPPING_INFORMATION_RESERVED.nof_rb_ra,
                  "The PRACH and PUSCH subcarrier spacing combination resulted in a reserved configuration.");
    srsran_assert(freq_mapping_info.k_bar != PRACH_FREQUENCY_MAPPING_INFORMATION_RESERVED.k_bar,
                  "The PRACH and PUSCH subcarrier spacing combination resulted in a reserved configuration.");

    dft_processor& dft                  = *dft_processors[preamble_info.scs];
    unsigned       prach_scs_Hz         = ra_scs_to_Hz(preamble_info.scs);
    unsigned       ofdm_symbol_len      = dft.get_size();
    unsigned       cyclic_prefix_length = preamble_info.cp_length.to_samples(srate.to_Hz());
    unsigned       nof_symbols          = preamble_info.symbol_length.to_samples(ra_scs_to_Hz(preamble_info.scs));

    // Make sure the number of occasions and symbols fit in the buffer.
    srsran_assert(nof_symbols <= buffer.get_max_nof_symbols(),
                  "The number of symbols (i.e., {}) must be less than or equal to the maximum number of symbols of the "
                  "buffer (i.e., {}).",
                  nof_symbols,
                  buffer.get_max_nof_symbols());

    // Prepare PRACH resource grid.
    unsigned K               = pusch_scs_Hz / prach_scs_Hz;
    unsigned prach_grid_size = config.nof_prb_ul_grid * K * NRE;
    srsran_assert(
        dft.get_size() > prach_grid_size,
        "DFT size {} for PRACH SCS {} is not sufficient for K={}, N_RB={}. The minimum expected DFT size is {}.",
        dft.get_size(),
        prach_scs_Hz,
        K,
        config.nof_prb_ul_grid,
        prach_grid_size);

    // Process each PRACH symbol.
    for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
      // Load symbol time-domain in the DFT.
      srsvec::copy(dft.get_input(),
                   input_occasion.subspan(cyclic_prefix_length + ofdm_symbol_len * i_symbol, ofdm_symbol_len));

      // Perform DFT.
      span<const cf_t> dft_output = dft.run();

      // For each frequency-domain occasion.
      for (unsigned i_fd_occasion = 0; i_fd_occasion != config.nof_fd_occasions; ++i_fd_occasion) {
        // Calculate initial subcarrier.
        unsigned k_start =
            K * NRE * (config.rb_offset + freq_mapping_info.nof_rb_ra * i_fd_occasion) + freq_mapping_info.k_bar;
        srsran_assert(k_start + preamble_info.sequence_length < prach_grid_size,
                      "Start subcarrier {} plus sequence length {} exceeds PRACH grid size {}.",
                      k_start,
                      preamble_info.sequence_length,
                      prach_grid_size);

        // Select destination PRACH symbol in the buffer.
        span<cf_t> prach_symbol = buffer.get_symbol(config.port, i_td_occasion, i_fd_occasion, i_symbol);

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
      }
    }
  }
}