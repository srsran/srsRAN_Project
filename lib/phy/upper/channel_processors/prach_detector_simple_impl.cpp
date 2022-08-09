/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_detector_simple_impl.h"
#include "srsgnb/ran/prach/prach_preamble_information.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/srsvec/prod.h"
#include "srsgnb/srsvec/zero.h"
#include "srsgnb/support/error_handling.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

prach_detection_result prach_detector_simple_impl::detect(const prach_buffer& input, const configuration& config)
{
  srsgnb_assert(config.start_preamble_index + config.nof_preamble_indices <= 64,
                "The start preamble index {} and the number of preambles to detect {}, exceed the maximum of 64.",
                config.start_preamble_index,
                config.nof_preamble_indices);

  // Retrieve preamble configuration.
  prach_preamble_information preamble_info = get_prach_preamble_long_info(config.format);

  // Verify sequence lengths match.
  srsgnb_assert(input.get_sequence_length() == preamble_info.sequence_length,
                "The input buffer sequence length {} is not equal to the expected preamble sequence length {}.",
                input.get_sequence_length(),
                preamble_info.sequence_length);
  unsigned sequence_length_lower = preamble_info.sequence_length / 2;
  unsigned sequence_length_upper = preamble_info.sequence_length - sequence_length_lower;

  // Derive time domain sampling rate in Hz.
  unsigned sampling_rate_Hz = preamble_info.scs.to_Hz() * idft->get_size();

  // Segment the IDFT input into lower grid, upper grid and guard.
  span<cf_t> idft_lower_grid = idft->get_input().last(sequence_length_lower);
  span<cf_t> idft_upper_grid = idft->get_input().first(sequence_length_upper);
  span<cf_t> idft_guard      = idft->get_input().subspan(divide_ceil(sequence_length_upper, 2),
                                                    idft->get_size() - preamble_info.sequence_length);

  // Set the IDFT guard to zero.
  srsvec::zero(idft_guard);

  // Calculate RSSI.
  float rssi = srsvec::average_power(input.get_symbol(0));

  // Prepare results.
  prach_detection_result result;
  result.rssi_dB         = convert_power_to_dB(rssi);
  result.time_resolution = phy_time_unit::from_seconds(1.0 / static_cast<double>(sampling_rate_Hz));
  result.preambles.clear();

  // Early stop if the RSSI is zero.
  if (!std::isnormal(rssi)) {
    return result;
  }

  // For each preamble to detect...
  for (unsigned preamble_index     = config.start_preamble_index,
                preamble_index_end = config.start_preamble_index + config.nof_preamble_indices;
       preamble_index != preamble_index_end;
       ++preamble_index) {
    // Generate preamble.
    prach_generator::configuration preamble_config;
    preamble_config.format                = config.format;
    preamble_config.root_sequence_index   = config.root_sequence_index;
    preamble_config.preamble_index        = preamble_index;
    preamble_config.restricted_set        = config.restricted_set;
    preamble_config.zero_correlation_zone = config.zero_correlation_zone;
    span<const cf_t> preamble_freq        = generator->generate(preamble_config);

    // Measure input signal power. Make sure an invalid power does not propagate.
    float preamble_power = srsvec::average_power(preamble_freq);
    report_fatal_error_if_not(std::isnormal(preamble_power), "Corrupted generated signal.");

    // Select first symbol in the buffer.
    span<const cf_t> signal_freq = input.get_symbol(0);

    // Perform correlation in frequency-domain and store the result in the IDFT input.
    srsvec::prod_conj(
        signal_freq.first(sequence_length_lower), preamble_freq.first(sequence_length_lower), idft_lower_grid);
    srsvec::prod_conj(
        signal_freq.last(sequence_length_upper), preamble_freq.last(sequence_length_upper), idft_upper_grid);

    // Convert the correlation to the time domain.
    span<const cf_t> correlation = idft->run();

    // Find delay and power of the maximum absolute value.
    std::pair<unsigned, float> max_abs   = srsvec::max_abs_element(correlation);
    unsigned                   delay_n   = max_abs.first;
    float                      max_power = max_abs.second;

    // Check if the maximum value gets over the threshold.
    float norm_corr = max_power / (rssi * preamble_power * idft->get_size() * idft->get_size());
    if (norm_corr < DETECTION_THRESHOLD) {
      continue;
    }

    result.preambles.emplace_back();
    prach_detection_result::preamble_indication& info = result.preambles.back();
    info.preamble_index                               = preamble_index;
    info.time_advance =
        phy_time_unit::from_seconds(static_cast<double>(delay_n) / static_cast<double>(sampling_rate_Hz));
    info.power_dB = convert_power_to_dB(max_power);
    info.snr_dB   = 0.0F;
  }

  return result;
}
