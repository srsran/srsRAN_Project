/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_detector_generic_impl.h"
#include "srsran/ran/prach/prach_cyclic_shifts.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/srsvec/add.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/division.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/modulus_square.h"
#include "srsran/srsvec/prod.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

bool prach_detector_validator_impl::is_valid(const prach_detector::configuration& config) const
{
  // Only unrestricted sets are supported.
  if (config.restricted_set != restricted_set_config::UNRESTRICTED) {
    return false;
  }

  // No special requirements for long formats.
  if (is_long_preamble(config.format)) {
    return true;
  }

  // Check short preambles configurations.
  if ((config.format != prach_format_type::A1) && (config.format != prach_format_type::B4)) {
    // Currently, only formats A1 and B4 are supported.
    return false;
  }

  if (config.ra_scs > prach_subcarrier_spacing::kHz30) {
    // Currently, only PRACH subcarrier spacings of 15 and 30 kHz are supported.
    return false;
  }

  // The zero correlation zone supported are the values indicated in TS38.104 Table A.6-1.
  if ((config.ra_scs == prach_subcarrier_spacing::kHz15) && (config.zero_correlation_zone != 0) &&
      (config.zero_correlation_zone != 11)) {
    return false;
  }
  if ((config.ra_scs == prach_subcarrier_spacing::kHz30) && (config.zero_correlation_zone != 0) &&
      (config.zero_correlation_zone != 14)) {
    return false;
  }
  return true;
}

static std::tuple<float, unsigned> get_th_and_margin(const prach_detector::configuration& config)
{
  if (config.nof_rx_ports == 1) {
    switch (config.format) {
      case prach_format_type::zero:
      case prach_format_type::one:
      case prach_format_type::two:
      case prach_format_type::three:
        // This threshold has been tuned for Format 0, ZCZ = 1, and one receive port.
        return {/* threshold = */ 2.0F, /* win_margin = */ 5};
      case prach_format_type::A1:
      case prach_format_type::B4:
        // This threshold has been tuned for Format B4, ZCZ = 11, and one receive port.
        return {/* threshold = */ 0.15F, /* win_margin = */ 12};
      default: // Do nothing.
        return {0.0F, 0};
    }
  }
  switch (config.format) {
    case prach_format_type::zero:
    case prach_format_type::one:
    case prach_format_type::two:
    case prach_format_type::three:
      // This threshold has been tuned for Format 0, ZCZ = 1, and two receive ports.
      return {/* threshold = */ 0.88F, /* win_margin = */ 5};
    case prach_format_type::A1:
    case prach_format_type::B4:
      // This threshold has been tuned for Format A1, ZCZ = 11, and two receive ports.
      return {/* threshold = */ 0.37F, /* win_margin = */ 12};
    default: // Do nothing.
      return {0.0F, 0};
  }
}

prach_detection_result prach_detector_generic_impl::detect(const prach_buffer& input, const configuration& config)
{
  srsran_assert(
      config.start_preamble_index + config.nof_preamble_indices <= prach_constants::MAX_NUM_PREAMBLES,
      "The start preamble index (i.e., {}) and the number of preambles to detect (i.e., {}), exceed the maximum of 64.",
      config.start_preamble_index,
      config.nof_preamble_indices);

  // Get preamble information.
  prach_preamble_information preamble_info = {};
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

  // Deduce sampling rate.
  double sample_rate_Hz = DFT_SIZE * ra_scs_to_Hz(preamble_info.scs);

  // Calculate cyclic prefix duration in seconds.
  double cp_duration = preamble_info.cp_length.to_seconds();

  // Calculate cyclic prefix duration in the same units as Ncs.
  unsigned cp_prach = static_cast<unsigned>(std::floor(cp_duration * L_ra * ra_scs_to_Hz(preamble_info.scs)));

  // Calculate the window width for each shift and convert it to the correlation sampling rate.
  unsigned win_width = std::min(N_cs, cp_prach);
  if (N_cs == 0) {
    win_width = cp_prach;
  }
  win_width = (win_width * DFT_SIZE) / L_ra;

  // Select window margin and threshold.
  auto     th_and_margin = get_th_and_margin(config);
  float    threshold     = std::get<0>(th_and_margin);
  unsigned win_margin    = std::get<1>(th_and_margin);
  srsran_assert((win_margin > 0) && (threshold > 0.0),
                "Window margin and threshold are not selected for the number of ports (i.e., {}) and the preamble "
                "format (i.e., {}).",
                config.nof_rx_ports,
                to_string(config.format));

  // Calculate maximum delay.
  unsigned guard             = 50;
  unsigned max_delay_samples = (N_cs == 0) ? cp_prach : std::min(std::max(N_cs, 1U) - 1U, cp_prach);
  max_delay_samples          = (max_delay_samples * DFT_SIZE) / L_ra;
  max_delay_samples          = std::max(guard + 1, max_delay_samples) - guard;

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
      phy_time_unit::from_seconds(static_cast<double>(max_delay_samples) / static_cast<double>(sample_rate_Hz));
  result.preambles.clear();

  // Early stop if the RSSI is zero.
  if (!std::isnormal(rssi)) {
    return result;
  }

  // Get view of the IDFT input and zero it.
  span<cf_t> idft_input = idft->get_input();
  srsvec::zero(idft_input);

  for (unsigned i_sequence = 0; i_sequence != nof_sequences; ++i_sequence) {
    // Prepare root sequence configuration.
    prach_generator::configuration generator_config = {};
    generator_config.format                         = config.format;
    generator_config.root_sequence_index            = config.root_sequence_index;
    generator_config.preamble_index                 = i_sequence * nof_shifts;
    generator_config.restricted_set                 = config.restricted_set;
    generator_config.zero_correlation_zone          = config.zero_correlation_zone;

    // Generate root sequence.
    span<const cf_t> root = generator->generate(generator_config);

    // Prepare metric global numerator.
    metric_global_num.resize({win_width, nof_shifts});
    srsvec::zero(metric_global_num.get_data());

    // Prepare metric global denominator.
    metric_global_den.resize({win_width, nof_shifts});
    srsvec::zero(metric_global_den.get_data());

    // Iterate over all replicas.
    for (unsigned i_port = 0; i_port != config.nof_rx_ports; ++i_port) {
      for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
        // Get view of the preamble.
        span<const cf_t> preamble = input.get_symbol(i_port, i_td_occasion, i_fd_occasion, i_symbol);

        // Multiply the preamble by the complex conjugate of the root sequence.
        std::array<cf_t, prach_constants::LONG_SEQUENCE_LENGTH> no_root_temp;
        span<cf_t>                                              no_root = span<cf_t>(no_root_temp).first(L_ra);
        srsvec::prod_conj(preamble, root, no_root);

        // Prepare IDFT for correlation.
        srsvec::copy(idft_input.first(L_ra / 2 + 1), no_root.last(L_ra / 2 + 1));
        srsvec::copy(idft_input.last(L_ra / 2), no_root.first(L_ra / 2));

        // Perform IDFT.
        span<const cf_t> no_root_time_simple = idft->run();

        // Perform the modulus square of the correlation.
        span<float> mod_square = span<float>(temp).first(DFT_SIZE);
        srsvec::modulus_square(mod_square, no_root_time_simple);

        // Normalize the signal: we divide by the DFT size to compensate for the inherent scaling of the DFT, and by
        // L_ra^2 to compensate for the amplitude of the ZC sequence in the frequency domain.
        srsvec::sc_prod(mod_square, 1.0F / static_cast<float>(DFT_SIZE * L_ra * L_ra), mod_square);

        // Process each shift of the sequence.
        for (unsigned i_window = 0; i_window != nof_shifts; ++i_window) {
          // Calculate the start of the window.
          unsigned window_start = (DFT_SIZE - (N_cs * i_window * DFT_SIZE) / L_ra) % DFT_SIZE;

          // Calculate reference energy.
          float reference = 0.0F;
          for (unsigned i = 0, i_end = 2 * win_margin + win_width; i != i_end; ++i) {
            unsigned i_mod_square = ((i + window_start + DFT_SIZE) - win_margin) % DFT_SIZE;
            reference += mod_square[i_mod_square];
          }

          // Select modulus square view for the window.
          span<float> window_mod_square = span<float>(temp2).first(win_width);

          // Scale window.
          srsvec::sc_prod(mod_square.subspan(window_start, win_width),
                          static_cast<float>(DFT_SIZE) / static_cast<float>(L_ra),
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
      auto* it = std::max_element(metric_global.begin(), metric_global.end());

      // Extract peak value and index from the iterator.
      float    peak  = *it;
      unsigned delay = static_cast<unsigned>(it - metric_global.begin());

      // Check with the threshold.
      if ((it != metric_global.end()) && (peak > threshold) && (delay < max_delay_samples)) {
        prach_detection_result::preamble_indication& info = result.preambles.emplace_back();
        info.preamble_index                               = i_sequence * nof_shifts + i_window;
        info.time_advance =
            phy_time_unit::from_seconds(static_cast<double>(delay) / static_cast<double>(sample_rate_Hz));
        info.power_dB = convert_power_to_dB(peak);
        info.snr_dB   = convert_power_to_dB(peak);
      }
    }
  }

  return result;
}
