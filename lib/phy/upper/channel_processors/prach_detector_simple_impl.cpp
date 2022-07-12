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
#include "prach_preamble_helpers.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/srsvec/prod.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

prach_detector::detection_result prach_detector_simple_impl::detect(span<const cf_t>                          signal,
                                                                    const prach_detector::slot_configuration& config)
{
  // Retrieve preamble configuration.
  prach_preamble_modulation_info preamble_info = prach_preamble_modulation_info_get_long(config.format, dft_size_15kHz);

  // Select DFT and IDFT.
  dft_processor* dft  = dft_1_25_kHz.get();
  dft_processor* idft = idft_1_25_kHz.get();
  if (preamble_info.scs == prach_subcarrier_spacing::kHz5) {
    dft  = dft_5_kHz.get();
    idft = idft_5_kHz.get();
  }

  // Select PRACH window.
  unsigned prach_window_offset = preamble_info.N_cp_ra + preamble_info.N_u - dft->get_size();
  unsigned prach_window_length = dft->get_size();

  // Copy PRACH window into window.
  srsran_assert(signal.size() >= prach_window_offset + prach_window_length,
                "The PRACH window with offset {} and length {} exceeds the signal buffer size {}.",
                prach_window_offset,
                prach_window_length,
                signal.size());
  srsvec::copy(dft->get_input(), signal.subspan(prach_window_offset, prach_window_length));

  // Measure input signal power.
  float rssi = srsvec::average_power(dft->get_input());
  if (!std::isnormal(rssi)) {
    return {};
  }

  detection_result result;
  result.first_symbol_index = 0;
  result.slot_index         = 0;
  result.ra_index           = 0;
  result.rssi_dB            = convert_power_to_dB(rssi);
  result.preambles.clear();

  // Convert signal into frequency domain and copy to temporal vector.
  span<cf_t> signal_freq = signal_freq_temp.first(dft->get_size());
  srsvec::copy(signal_freq, dft->run());

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
    preamble_config.rb_offset             = config.frequency_offset;
    preamble_config.pusch_scs             = config.pusch_scs;
    preamble_config.frequency_domain      = true;
    span<const cf_t> preamble_freq        = generator->generate(preamble_config);

    // Measure input signal power.
    float preamble_power = srsvec::average_power(preamble_freq);
    if (!std::isnormal(preamble_power)) {
      return {};
    }

    // Perform correlation in frequency-domain and store in the IDFT input.
    srsvec::prod_conj(signal_freq, preamble_freq, idft->get_input());

    // Convert to correlation to time-domain.
    span<const cf_t> correlation = idft->run();

    // Find delay and power of the maximum absolute value.
    std::pair<unsigned, float> max_abs   = srsvec::max_abs_element(correlation);
    unsigned                   delay_n   = max_abs.first;
    float                      max_power = max_abs.second;

    // Check if the maximum value gets over the threshold.
    float norm_corr = max_power / (rssi * preamble_power * idft->get_size() * idft->get_size() * idft->get_size());
    if (norm_corr < DETECTION_THRESHOLD) {
      continue;
    }

    result.preambles.emplace_back();
    preamble_indication& info = result.preambles.back();
    info.preamble_index       = preamble_index;
    info.time_advance_us      = static_cast<float>(delay_n) / (15e3F * static_cast<float>(dft_size_15kHz) * 1e-6F);
    info.power_dB             = convert_power_to_dB(max_power);
    info.snr_dB               = 0.0F;
  }

  return result;
}
