/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "amplitude_controller_impl.h"
#include "srsgnb/srsvec/clip.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

amplitude_controller_metrics amplitude_controller_impl::process(span<cf_t> output, span<const cf_t> input)
{
  srsgnb_srsvec_assert_size(output, input);

  // Compute Mean and Peak signal power.
  float avg_power         = srsvec::average_power(input);
  float peak_amplitude    = std::sqrt(srsvec::max_abs_element(input).second);
  metrics.avg_power_dBFS  = convert_power_to_dB(avg_power / (full_scale_lin * full_scale_lin));
  metrics.peak_power_dBFS = convert_power_to_dB(peak_amplitude / full_scale_lin);

  // Compute signal PAPR.
  if ((std::isnormal(metrics.avg_power_dBFS)) && (std::isnormal(metrics.peak_power_dBFS))) {
    metrics.papr_dB = metrics.peak_power_dBFS - metrics.avg_power_dBFS;
  } else {
    metrics.papr_dB = 0.0F;
  }

  // Apply the gain factor.
  metrics.gain_dB = convert_amplitude_to_dB(gain_lin);
  srsvec::sc_prod(input, gain_lin, output);

  // Clip the signal and count the number of clipped and processed samples.
  if (clipping_enabled) {
    metrics.nof_processed_samples += input.size();
    if (peak_amplitude * gain_lin > ceiling_lin) {
      metrics.nof_clipped_samples += srsvec::clip(output, output, ceiling_lin);
    }
    metrics.clipping_probability =
        (long double)metrics.nof_clipped_samples / (long double)metrics.nof_processed_samples;
  } else {
    metrics.clipping_probability = 0.0F;
  }

  return metrics;
}
