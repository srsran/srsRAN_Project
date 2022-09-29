/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "amplitude_controller_clipping_impl.h"
#include "srsgnb/srsvec/clip.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

amplitude_controller_metrics amplitude_controller_clipping_impl::process(span<cf_t> output, span<const cf_t> input)
{
  srsgnb_srsvec_assert_size(output, input);

  // Compute Mean and Peak signal power.
  float avg_power         = srsvec::average_power(input);
  float peak_power        = srsvec::max_abs_element(input).second;
  metrics.avg_power_dBFS  = convert_power_to_dB(avg_power / full_scale_pwr);
  metrics.peak_power_dBFS = convert_power_to_dB(peak_power / full_scale_pwr);

  // Compute signal PAPR.
  if ((std::isnormal(metrics.avg_power_dBFS)) && (std::isnormal(metrics.peak_power_dBFS))) {
    metrics.papr_dB = metrics.peak_power_dBFS - metrics.avg_power_dBFS;
  } else {
    metrics.papr_dB = 0.0F;
  }

  // Apply the gain factor.
  metrics.gain_dB = convert_amplitude_to_dB(amplitude_gain);
  srsvec::sc_prod(input, amplitude_gain, output);

  // Clip the signal and count the number of clipped and processed samples.
  if (clipping_enabled) {
    metrics.nof_processed_samples += input.size();
    metrics.nof_clipped_samples += srsvec::clip_iq(output, output, ceiling_lin);
    metrics.clipping_probability =
        static_cast<double>(metrics.nof_clipped_samples) / static_cast<double>(metrics.nof_processed_samples);
  } else {
    metrics.clipping_probability = 0.0F;
  }

  return metrics;
}
