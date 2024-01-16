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

#include "prach_detector_generic_impl.h"
#include "prach_detector_generic_thresholds.h"
#include "srsran/adt/interval.h"
#include "srsran/ran/prach/prach_cyclic_shifts.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/srsvec/accumulate.h"
#include "srsran/srsvec/add.h"
#include "srsran/srsvec/compare.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/division.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/modulus_square.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

static const detail::threshold_and_margin_finder threshold_and_margin_table(detail::all_threshold_and_margins);

bool prach_detector_validator_impl::is_valid(const prach_detector::configuration& config) const
{
  detail::threshold_params th_params;
  th_params.nof_rx_ports          = config.nof_rx_ports;
  th_params.scs                   = config.ra_scs;
  th_params.format                = config.format;
  th_params.zero_correlation_zone = config.zero_correlation_zone;
  th_params.combine_symbols       = true;

  auto flag = threshold_and_margin_table.check_flag(th_params);

  return (flag != detail::threshold_and_margin_finder::threshold_flag::red);
}

prach_detector_generic_impl::prach_detector_generic_impl(std::unique_ptr<dft_processor>   idft_long_,
                                                         std::unique_ptr<dft_processor>   idft_short_,
                                                         std::unique_ptr<prach_generator> generator_,
                                                         bool                             combine_symbols_) :
  temp(),
  temp2(),
  idft_long(std::move(idft_long_)),
  idft_short(std::move(idft_short_)),
  generator(std::move(generator_)),
  combine_symbols(combine_symbols_)
{
  static constexpr interval<unsigned, true> idft_long_sz_range(prach_constants::LONG_SEQUENCE_LENGTH, MAX_IDFT_SIZE);
  static constexpr interval<unsigned, true> idft_short_sz_range(prach_constants::SHORT_SEQUENCE_LENGTH, MAX_IDFT_SIZE);

  // Verify IDFT for long preambles.
  srsran_assert(idft_long, "Invalid IDFT processor.");
  srsran_assert(idft_long->get_direction() == dft_processor::direction::INVERSE, "Expected IDFT.");
  srsran_assert(idft_long_sz_range.contains(idft_long->get_size()),
                "IDFT size for long preambles (i.e., {}) must be in range {}.",
                idft_long->get_size(),
                idft_long_sz_range);

  // Verify IDFT for short preambles.
  srsran_assert(idft_short, "Invalid IDFT processor.");
  srsran_assert(idft_short->get_direction() == dft_processor::direction::INVERSE, "Expected IDFT.");
  srsran_assert(idft_short_sz_range.contains(idft_short->get_size()),
                "IDFT size for short preambles (i.e., {}) must be in range {}.",
                idft_short->get_size(),
                idft_long_sz_range);
};

prach_detection_result prach_detector_generic_impl::detect(const prach_buffer& input, const configuration& config)
{
  srsran_assert(config.start_preamble_index + config.nof_preamble_indices <= prach_constants::MAX_NUM_PREAMBLES,
                "The start preamble index (i.e., {}) and the number of preambles to detect (i.e., {}), exceed the "
                "maximum of 64.",
                config.start_preamble_index,
                config.nof_preamble_indices);

  // Get preamble information.
  prach_preamble_information preamble_info;
  if (is_long_preamble(config.format)) {
    preamble_info = get_prach_preamble_long_info(config.format);
  } else {
    preamble_info = get_prach_preamble_short_info(config.format, config.ra_scs, false);
  }

  // Get cyclic shift.
  unsigned N_cs = prach_cyclic_shifts_get(config.ra_scs, config.restricted_set, config.zero_correlation_zone);
  srsran_assert(N_cs != PRACH_CYCLIC_SHIFTS_RESERVED, "Reserved cyclic shift.");

  // Get sequence length.
  unsigned L_ra = prach_constants::LONG_SEQUENCE_LENGTH;
  if (is_short_preamble(config.format)) {
    L_ra = prach_constants::SHORT_SEQUENCE_LENGTH;
  }

  // Gets the number of preambles using the same root sequence (i.e., the number of cyclic shifts) and the total
  // number of root sequences needed.
  unsigned nof_shifts    = 1;
  unsigned nof_sequences = 64;
  if (N_cs != 0) {
    nof_shifts    = std::min(prach_constants::MAX_NUM_PREAMBLES, L_ra / N_cs);
    nof_sequences = divide_ceil(64, nof_shifts);
  }

  // Select IDFT processor.
  dft_processor& idft = is_long_preamble(config.format) ? *idft_long : *idft_short;

  // Get DFT size.
  unsigned dft_size = idft.get_size();

  // Deduce sampling rate.
  double sample_rate_Hz = dft_size * ra_scs_to_Hz(preamble_info.scs);

  // Calculate cyclic prefix duration in seconds.
  double cp_duration = preamble_info.cp_length.to_seconds();

  // Calculate cyclic prefix duration in the same units as Ncs.
  unsigned cp_prach = static_cast<unsigned>(std::floor(cp_duration * L_ra * ra_scs_to_Hz(preamble_info.scs)));

  // Calculate the window width for each shift and convert it to the correlation sampling rate.
  unsigned win_width = std::min(N_cs, cp_prach);
  if (N_cs == 0) {
    win_width = cp_prach;
  }
  win_width = (win_width * dft_size) / L_ra;

  // Select window margin and threshold.
  detail::threshold_params th_params;
  th_params.nof_rx_ports          = config.nof_rx_ports;
  th_params.scs                   = config.ra_scs;
  th_params.format                = config.format;
  th_params.zero_correlation_zone = config.zero_correlation_zone;
  th_params.combine_symbols       = combine_symbols;

  auto     th_and_margin = threshold_and_margin_table.get(th_params);
  float    threshold     = std::get<0>(th_and_margin);
  unsigned win_margin    = std::get<1>(th_and_margin);
  srsran_assert((win_margin > 0) && (threshold > 0.0),
                "Window margin and threshold are not selected for the number of ports (i.e., {}) and the preamble "
                "format (i.e., {}).",
                config.nof_rx_ports,
                to_string(config.format));

  // Calculate maximum delay.
  unsigned max_delay_samples = (N_cs == 0) ? cp_prach : std::min(std::max(N_cs, 1U) - 1U, cp_prach);
  max_delay_samples          = (max_delay_samples * dft_size) / L_ra;

  // Calculate number of symbols.
  unsigned nof_symbols =
      static_cast<unsigned>(preamble_info.symbol_length.to_seconds() * ra_scs_to_Hz(preamble_info.scs));

  unsigned i_td_occasion = 0;
  unsigned i_fd_occasion = 0;

  // Calculate RSSI.
  float rssi = 0.0F;
  for (unsigned i_port = 0; i_port != config.nof_rx_ports; ++i_port) {
    for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
      rssi += srsvec::average_power(input.get_symbol(i_port, i_td_occasion, i_fd_occasion, i_symbol));
    }
  }
  rssi /= static_cast<float>(config.nof_rx_ports * nof_symbols * L_ra);

  // Prepare results.
  prach_detection_result result;
  result.rssi_dB         = convert_power_to_dB(rssi);
  result.time_resolution = phy_time_unit::from_seconds(1.0 / static_cast<double>(sample_rate_Hz));
  result.time_advance_max =
      phy_time_unit::from_seconds(static_cast<double>(max_delay_samples) * 0.8 / static_cast<double>(sample_rate_Hz));
  result.preambles.clear();

  // Early stop if the RSSI is zero.
  if (!std::isnormal(rssi)) {
    return result;
  }

  // Get view of the IDFT input and zero it.
  span<cf_t> idft_input = idft.get_input();
  srsvec::zero(idft_input);

  for (unsigned i_sequence = 0; i_sequence != nof_sequences; ++i_sequence) {
    // Prepare root sequence configuration.
    prach_generator::configuration generator_config;
    generator_config.format                = config.format;
    generator_config.root_sequence_index   = config.root_sequence_index;
    generator_config.preamble_index        = i_sequence * nof_shifts;
    generator_config.restricted_set        = config.restricted_set;
    generator_config.zero_correlation_zone = config.zero_correlation_zone;

    // Generate root sequence.
    span<const cf_t> root = generator->generate(generator_config);

    // Prepare metric global numerator.
    metric_global_num.resize({win_width, nof_shifts});
    srsvec::zero(metric_global_num.get_data());

    // Prepare metric global denominator.
    metric_global_den.resize({win_width, nof_shifts});
    srsvec::zero(metric_global_den.get_data());

    // Iterate over all receive ports.
    for (unsigned i_port = 0; i_port != config.nof_rx_ports; ++i_port) {
      // Iterate all PRACH symbols if they are not combined, otherwise process only one PRACH symbol.
      for (unsigned i_symbol = 0, i_symbol_end = (combine_symbols) ? 1 : nof_symbols; i_symbol != i_symbol_end;
           ++i_symbol) {
        // Get view of the preamble.
        span<const cf_t> preamble = input.get_symbol(i_port, i_td_occasion, i_fd_occasion, i_symbol);

        // Combine symbols.
        if (combine_symbols && (nof_symbols > 1)) {
          // Get a temporary destination for the symbol combination.
          span<cf_t> combined_symbols = span<cf_t>(cf_temp).first(L_ra);

          // Copy the first PRACH symbol.
          srsvec::copy(combined_symbols, preamble);

          // Combine the rest of PRACH symbols.
          for (unsigned i_comb_symbol = 1; i_comb_symbol != nof_symbols; ++i_comb_symbol) {
            srsvec::add(combined_symbols,
                        input.get_symbol(i_port, i_td_occasion, i_fd_occasion, i_comb_symbol),
                        combined_symbols);
          }

          preamble = combined_symbols;
        }

        // Multiply the preamble by the complex conjugate of the root sequence.
        std::array<cf_t, prach_constants::LONG_SEQUENCE_LENGTH> no_root_temp;
        span<cf_t>                                              no_root = span<cf_t>(no_root_temp).first(L_ra);
        srsvec::prod_conj(preamble, root, no_root);

        // Prepare IDFT for correlation.
        srsvec::copy(idft_input.first(L_ra / 2 + 1), no_root.last(L_ra / 2 + 1));
        srsvec::copy(idft_input.last(L_ra / 2), no_root.first(L_ra / 2));

        // Perform IDFT.
        span<const cf_t> no_root_time_simple = idft.run();

        // Perform the modulus square of the correlation.
        span<float> mod_square = span<float>(temp).first(dft_size);
        srsvec::modulus_square(mod_square, no_root_time_simple);

        // Normalize the signal: we divide by the DFT size to compensate for the inherent scaling of the DFT, and by
        // L_ra^2 to compensate for the amplitude of the ZC sequence in the frequency domain.
        srsvec::sc_prod(mod_square, 1.0F / static_cast<float>(dft_size * L_ra * L_ra), mod_square);

        // Process each shift of the sequence.
        for (unsigned i_window = 0; i_window != nof_shifts; ++i_window) {
          // Calculate the start of the window.
          unsigned window_start = (dft_size - (N_cs * i_window * dft_size) / L_ra) % dft_size;

          // Calculate reference energy.
          float reference = 0.0F;
          {
            unsigned i_start = ((window_start + dft_size) - win_margin) % dft_size;
            unsigned i_end   = i_start + 2 * win_margin + win_width;
            reference += srsvec::accumulate(mod_square.subspan(i_start, std::min(i_end, dft_size) - i_start));
            if (i_end > dft_size) {
              reference += srsvec::accumulate(mod_square.first(i_end - dft_size));
            }
          }

          // Select modulus square view for the window.
          span<float> window_mod_square = span<float>(temp2).first(win_width);

          // Scale window.
          srsvec::sc_prod(mod_square.subspan(window_start, win_width),
                          static_cast<float>(dft_size) / static_cast<float>(L_ra),
                          window_mod_square);

          // Select metric global numerator.
          span<float> window_metric_global_num = metric_global_num.get_view({i_window});

          // Select metric global denominator.
          span<float> window_metric_global_den = metric_global_den.get_view({i_window});

          // Scale modulus square and accumulate numerator.
          srsvec::add(window_metric_global_num, window_mod_square, window_metric_global_num);

          // Scale modulus square and accumulate denominator.
          vector_noise_estimation(window_metric_global_den, reference, window_mod_square);
        }
      }
    }

    // Process global metric.
    for (unsigned i_window = 0; i_window != nof_shifts; ++i_window) {
      // Select metric global.
      span<float> metric_global            = span<float>(temp).first(win_width);
      span<float> window_metric_global_num = metric_global_num.get_view({i_window});
      span<float> window_metric_global_den = metric_global_den.get_view({i_window});

      // Process metric global.
      for (float& a : window_metric_global_den) {
        a = std::abs(a);
      }
      srsvec::divide(metric_global, window_metric_global_num, window_metric_global_den);

      // Find maximum.
      std::pair<unsigned, float> max_element = srsvec::max_element(metric_global);

      // Extract peak value and index from the iterator.
      unsigned delay = max_element.first;
      float    peak  = max_element.second;

      // Compare with the threshold. Note that we neglect the last 1/5 of the detection window because it may contain
      // spurious peaks due to the adjacent window.
      if ((delay < metric_global.size()) && (peak > threshold) &&
          (delay < static_cast<float>(max_delay_samples) * 0.8)) {
        prach_detection_result::preamble_indication& info = result.preambles.emplace_back();
        info.preamble_index                               = i_sequence * nof_shifts + i_window;
        info.time_advance =
            phy_time_unit::from_seconds(static_cast<double>(delay) / static_cast<double>(sample_rate_Hz));
        // Normalize the detection metric with respect to the threshold.
        info.detection_metric = peak / threshold;
      }
    }
  }

  return result;
}
