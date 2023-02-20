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

using namespace srsran;

amplitude_controller_metrics amplitude_controller_clipping_impl::process(span<cf_t> output, span<const cf_t> input)
{
  srsgnb_srsvec_assert_size(output, input);

  // Report clipping status.
  metrics.clipping_enabled = clipping_enabled;

  // Apply the gain factor.
  metrics.gain_dB = convert_amplitude_to_dB(amplitude_gain);
  srsvec::sc_prod(input, amplitude_gain, output);

  // Compute Mean and Peak signal power.
  float avg_power  = srsvec::average_power(output);
  float peak_power = srsvec::max_abs_element(output).second;

  // Normalize power to full scale.
  metrics.avg_power_fs  = avg_power / full_scale_pwr;
  metrics.peak_power_fs = peak_power / full_scale_pwr;

  if ((!std::isnormal(avg_power)) || (!std::isnormal(peak_power))) {
    // If the signal has zero power, do nothing.
    metrics.papr_lin = 1.0F;
    return metrics;
  }

  // Compute signal PAPR.
  metrics.papr_lin = peak_power / avg_power;

  if (clipping_enabled) {
    // Clip the signal and count the number of clipped and processed samples.
    metrics.nof_processed_samples += input.size();
    metrics.nof_clipped_samples += srsvec::clip_iq(output, output, ceiling_lin);
    metrics.clipping_probability =
        static_cast<double>(metrics.nof_clipped_samples) / static_cast<double>(metrics.nof_processed_samples);
  }

  return metrics;
}
